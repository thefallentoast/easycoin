from protocol import ProtocolHandler
from sensitive import get_sensitive_info
import mysql.connector

class UserDatabaseBridge:
    '''
    Bridge between user access (from the server) and the MariaDB database.
    '''
    def __init__(self, connection_info: dict[str, str, str, str]) -> None:
        '''
        Initialize the bridge, and connect to the database.
        Arguments:
            connection_info: dict with 4 str's
                The information for connecting to the database. It should have the keys
                "host" (typically set to "localhost"), "user" (typically root or some 
                other username), "password" (the password for connecting), and 
                (optional) "database" (the database name, not required if there's only
                one in the server.)
        '''
        self.connection_info = connection_info

        self.conn = mysql.connector.connect(**self.connection_info)
        self.cursor = self.conn.cursor()
    #TODO

class Orchestrator:
    def __init__(self, address: str = "0.0.0.0", port: int = 5870) -> None:
        '''
        Initializes the orchestrator server.
        '''
        self.address, self.port = address, port
        
        
        self.bridge = UserDatabaseBridge(get_sensitive_info())
        self.protocol_handler = ProtocolHandler(self.address, self.port)
