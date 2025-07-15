import socket as sock
from collections.abc import Callable

'''
PROTOCOL DEFINITION: SEMISTRATUM
Maximum message size: 128 bytes
Messages must be padded with \x00 if 128 bytes weren't reached.

1. Authentication
Miner  -> Server: AUTH,<wallet>,<rig_name>,<software_name>,<protocol_version>
IF protocol version too old:
Server -> Miner : FAIL,1,Your protocol version is too old.
IF wallet invalid:
Server -> Miner : FAIL,2,Your wallet is invalid.
IF rig name not unique for this wallet:
Server -> Miner : FAIL,4,Your rig name is not unique for this wallet.
ELSE:
Server -> Miner : OK,0,Authenticated successfully.

2. Job request
Miner  -> Server: JOB
IF miner not authorized yet:
Server -> Miner : FAIL,1,You aren't authorized yet.
ELSE:
Server -> Miner : OK,<miner_hash>,<difficulty>,<job_id>

3. Share submission
Miner  -> Server: SUBMIT,<nonce>,<hashrate>,<job_id>
IF share is of low difficulty:
Server -> Miner : BAD,Low difficulty share.
IF nonce didn't pass verification:
Server -> Miner : BAD,Invalid nonce.
IF share is invalid for reason not listed here:
Server -> Miner : BAD<,optional message>
IF share valid:
Server -> Miner : GOOD
IF block found:
Server -> Miner : BLOCK<block height>

4. Disconnect
Miner -> Server: LEAVE
WHEN disconnect and deauthorize ok:
Server -> Miner: OK
'''

class ProtocolHandler:
    def __init__(self, address: str = "0.0.0.0", port = 5870) -> None:
        self.listener = sock.socket(sock.AF_INET, sock.SOCK_STREAM)
        self.address = address
        self.port = port
        self.request_handlers = {}

        self.listener.bind((self.address, self.port))
    def add_handler(self, request_type: str, handler: Callable):
        '''
        Adds a handler for each type of request.
        Arguments:
            request_type: str
                The type of request to add.
            handler: Callable
                The handler callable. Note that this should be passed without
                parenthesis, like in
                ```python
                prot = ProtocolHandler()
                def foo():
                    return 3.14159
                prot.add_handler("FOO", foo) # Note that foo doesn't have the parenthesis here
                ```
                The handler function must accept as many arguments as are expected
                to be received in the request. If the "FOO" request (in the example) is meant to
                take ONE argument (seperated from "FOO" by a comma, as in "FOO,1234"), the function
                foo must also accept one argument.
                
        '''
        self.request_handlers[request_type] = handler
    def wait_for_connection(self) -> tuple[sock.socket, tuple[str, int]]:
        '''
        Waits for a connection request (essentially self.listener.accept())
        Returns:
            Tuple containing the socket that was created after accepting the
            connection, and the address of the source of the request.
        '''
        return self.listener.accept()
    def parse_request(self, request: bytes | str, exec_handler: bool = True) -> any:
        '''
        Parses the request received from said socket.
        If exec_handler is set to True, it automatically executes the specified
        handler defined in self.request_handlers. You may add one using the
        add_handler function.
        Arguments:
            request: bytes or str
                Whatever is received from the socket (e.g from sock.recv()).
                If required, it may be decoded before the function, for that
                pass it as a string.
        Returns:
            None if exec_handler is False
            The value the handler returns if exec_handler is True.


        '''
        if   isinstance(request, str):
            request_split = request.split(",")
        elif isinstance(request, bytes):
            request_split = request.decode("utf-8").split(",")
        
        request_type = request_split[0]

        if exec_handler:
            return self.request_handlers[request_type](*request_split[1:])
        else:
            return None