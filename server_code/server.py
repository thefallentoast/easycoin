from protocol import ProtocolHandler
from sensitive import get_sensitive_info
from database import UserDatabaseBridge

class Orchestrator:
    def __init__(self, address: str = "0.0.0.0", port: int = 5870) -> None:
        '''
        Initializes the orchestrator server.
        '''
        self.address, self.port = address, port
        
        self.
        self.bridge = UserDatabaseBridge(get_sensitive_info("user"))
        self.protocol_handler = ProtocolHandler(self.address, self.port)
    
