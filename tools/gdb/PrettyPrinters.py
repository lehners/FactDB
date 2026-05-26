from typing import Iterator, Tuple, Type, Dict, Optional
import gdb


class ValuePrinter:
    """Base class to print a GDB value.
    We register all classes implementing ValuePrinter with their supported types.
    Usage:
    class PrinterImpl(ValuePrinter):
        @staticmethod
        def supported_types()-> [str]:
            return ["supported type"]
    """

    val: gdb.Value
    """The value to print"""

    def __init__(self, val: gdb.Value) -> None:
        """Constructor"""
        self.val = val


# ----------------------------------------------------------------------------

class TypePrinter(ValuePrinter):
    @staticmethod
    def supported_types() -> [str]:
        """Get the supported types"""
        return ["factDB::schemac::Type"]

    def to_string(self) -> str:
        """Format the value as a string"""
        tclass = str(self.val['tclass']).strip('"')[len('factDB::schemac::Type::') + 1:]

        print(f"GDBPPP: {tclass}")

        if tclass in ['Numeric']:
            length = int(self.val['length'])
            precision = int(self.val['precision'])
            return f"{tclass}<length: {length}, prec: {precision}>"
        elif tclass in ['Char', 'Varchar']:
            return f"{tclass}<{int(self.val['length'])}>"
        else:
            return tclass


# ----------------------------------------------------------------------------

class IUPrinter(ValuePrinter):
    column: str
    table: str

    def __init__(self, val: gdb.Value) -> None:
        """Constructor"""

        print("GDBPP: init IU")
        super().__init__(val)
        self.column = self.val['column']
        self.table = self.val['table']

    @staticmethod
    def supported_types() -> [str]:
        """Get the supported types"""
        return ["factDB::IU"]

    def to_string(self) -> str:
        """Format the value as a string"""
        stripped_table = self.table.strip('\"')
        stripped_column = self.column.strip("\"")
        type_str = str(self.val['type'])
        return f'{stripped_table}_{stripped_column} (Type: {type_str})'

    def children(self) -> Iterator[Tuple[str, gdb.Value]]:
        yield "column", self.column
        yield "table", self.table
        yield "type", self.val['type']


# ----------------------------------------------------------------------------

class BitSet64Printer(ValuePrinter):
    def __init__(self, val: gdb.Value) -> None:
        """Constructor"""
        super().__init__(val)

    @staticmethod
    def supported_types() -> [str]:
        """Get the supported types"""
        return ["factDB::infra::BitSet64"]

    def to_string(self) -> str:
        """Format the value as a string"""
        bitset_array = []
        bitset_value = int(self.val["set"])
        capacity = int(self.val["capacity"])
        for i in range(0, capacity):
            if (1 << i) & bitset_value != 0:
                bitset_array.append(i)

        return str(bitset_array)

    def children(self) -> Iterator[Tuple[str, gdb.Value]]:
        yield "set", int(self.val["set"])
        yield "capacity", int(self.val["capacity"])


# ----------------------------------------------------------------------------

class BaseTablePlanPrinter(ValuePrinter):
    def __init__(self, val: gdb.Value) -> None:
        """Constructor"""
        super().__init__(val)
        if self.val.type.code == gdb.TYPE_CODE_PTR:
            self.val = self.val.dereference()

    @staticmethod
    def supported_types() -> [str]:
        """Get the supported types"""
        return ["factDB::opt::BaseTablePlan", "^factDB::opt::BaseTablePlan"]

    def to_string(self) -> str:
        """Format the value as a string"""
        return str("Scan Table ") + str(BitSet64Printer(self.val["coveredRelations"]).to_string()[1:-1])

    def children(self) -> Iterator[Tuple[str, gdb.Value]]:
        fields = self.val.type.strip_typedefs().fields()
        for field in fields:
            yield field.name, self.val[field]


# ----------------------------------------------------------------------------


class JoinPlanPrinter(ValuePrinter):
    def __init__(self, val: gdb.Value) -> None:
        """Constructor"""
        super().__init__(val)
        if self.val.type.code == gdb.TYPE_CODE_PTR:
            self.val = self.val.dereference()

    @staticmethod
    def supported_types() -> [str]:
        """Get the supported types"""
        return ["factDB::opt::JoinPlan", "^factDB::opt::JoinPlan"]

    def to_string(self) -> str:
        """Format the value as a string"""
        leftPlan =  BitSet64Printer(self.val["left"]["coveredRelations"]).to_string()
        rightPlan = BitSet64Printer(self.val["right"]["coveredRelations"]).to_string()
        joinType = str(self.val["joinType"])[len("factDB::algebra::JoinType::"):]
        return str(joinType) + "Join: " + str(leftPlan) + str(" and ") + str(rightPlan)

    def children(self) -> Iterator[Tuple[str, gdb.Value]]:
        fields = self.val.type.strip_typedefs().fields()
        for field in fields:
            yield field.name, self.val[field]


# ----------------------------------------------------------------------------

class PlanPrinter(ValuePrinter):
    def __init__(self, val: gdb.Value) -> None:
        """Constructor"""
        super().__init__(val)
        if self.val.type.code == gdb.TYPE_CODE_PTR:
            self.val = self.val.dereference()

    @staticmethod
    def supported_types() -> [str]:
        """Get the supported types"""
        return ["factDB::opt::Plan", "^factDB::opt::Plan$"]

    def to_string(self) -> str:
        """Format the value as a string"""
        str_type = str(self.val["type"])
        if str_type == "factDB::opt::BaseTable":
            return BaseTablePlanPrinter(self.val).to_string()
        elif str_type == "factDB::opt::Join":
            return JoinPlanPrinter(self.val).to_string()
        else:
            return str(f"unknown Plan Type: {str_type}")

    def children(self) -> Iterator[Tuple[str, gdb.Value]]:
        fields = self.val.type.strip_typedefs().fields()
        for field in fields:
            yield field.name, self.val[field]


# ----------------------------------------------------------------------------

class PrinterDispatch(gdb.printing.PrettyPrinter):
    """Dispatcher for our printers"""

    lookup: Dict[str, Type[ValuePrinter]]
    """The dictionary mapping base types to value printer types"""

    def __init__(self) -> None:
        """Constructor"""
        super().__init__("umbra")
        self.lookup = {}

    def add_printer(self, printer: Type[ValuePrinter]) -> None:
        """Add the printer for its supported types"""
        try:
            for supported_type in printer.supported_types():
                self.lookup[supported_type] = printer
        except:
            pass

    @staticmethod
    def get_typename(val: gdb.Value) -> Optional[str]:
        """Extract the type name from a GDB value"""
        try:
            base_type = val.type
            if val.type.code == gdb.TYPE_CODE_REF:
                base_type = base_type.target
            return base_type.unqualified().strip_typedefs().tag
        except AttributeError:
            return None

    @staticmethod
    def strip_templates(typename: str) -> str:
        """Strip templates from the given typename"""
        # Note: this is wrong for nested templates, e.g., Foo<T>::Bar<U>, but we don't use them anyway
        index = typename.find('<')
        return typename if index == -1 else typename[:index]

    def __call__(self, val: gdb.Value) -> Optional[ValuePrinter]:
        typename = self.get_typename(val)
        if not typename:
            return None
        basetype = self.strip_templates(typename)
        if basetype not in self.lookup:
            return None
        return self.lookup[basetype](val)


# ----------------------------------------------------------------------------
printer_dispatch: PrinterDispatch
"""The dispatcher instance"""


def get_subclasses(base):
    yield base
    for s in base.__subclasses__():
        yield from get_subclasses(s)


def register_factdb_printers():
    """Register ValuePrinters with GDB"""

    global printer_dispatch
    printer_dispatch = PrinterDispatch()

    for printer in get_subclasses(ValuePrinter):
        printer_dispatch.add_printer(printer)

    gdb.printing.register_pretty_printer(None, printer_dispatch)
