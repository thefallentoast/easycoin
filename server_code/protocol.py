import socket as sock

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
    def __init__(self, address) -> None:
        self.listener = sock.socket(sock.AF_INET, sock.SOCK_STREAM)
        self.address = address
        self.request_handlers = {}

        self.listener.bind(self.address)
    def add_handler(request_type: str, handler: function)
    def wait_for_connection(self) -> tuple[sock.socket, sock._RetAddress]:
        '''
        Waits for a connection request (essentially self.listener.accept())
        Returns:
            Tuple containing the socket that was created after accepting the
            connection, and the address of the source of the request.
        '''
        return self.listener.accept()
    def parse_request(self, request: bytes | str):
        '''
        Parses the request received from said socket.
        Arguments:
            request: bytes or string
                Whatever is received from the socket (e.g from sock.recv()).
                If required, it may be decoded before the function, for that
                pass it as a string.
        '''
        if   isinstance(request, str):
            request_split = request.split(",")
        elif isinstance(request, bytes):
            request_split = request.decode("utf-8").split(",")
        
        request_type = request_split[0]

        if request_type == "AUTH":
            # Arguments: AUTH,<wallet>,<rig_name>,<software_name>,<protocol_version>
            