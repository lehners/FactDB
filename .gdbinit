python

import sys
sys.path.insert(0, 'tools/gdb')
from PrettyPrinters import register_factdb_printers

register_factdb_printers()

end
