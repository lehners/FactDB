
from bench.py.drivers.umbra import UmbraDriver
from pathlib import Path


class UmbraCEDriver(UmbraDriver):
    image = "umbra-release:7be3269b0"
    umbra_sql = "umbra_sql"
    db_local_path = Path("db/umbra_le")


DRIVER = UmbraCEDriver()
