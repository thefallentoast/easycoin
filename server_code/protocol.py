import socket as sock
from collections.abc import Callable
import hashlib

'''
SEMISTRATUM+ PROTOCOL
Maximum message size: 256 bytes
Messages encoded in: UTF-8

Note: MSSR means "Miner Send, Server Receive" and is equivalent to Miner -> Server
Note: SSMR means "Server Send, Miner Receive" and is equivalent to Server -> Miner

1. Authentication
Miner -> Server: AUTH,<wallet>,<rig_name>,<software_name>,<protocol_version>
Server -> Miner:
  IF protocol version too old: FAIL,1,Your protocol version is too old.
  IF wallet invalid: FAIL,2,Your wallet is invalid.
  IF rig name not unique for this wallet: FAIL,4,Your rig name is not unique for this wallet.
  ELSE: OK,0,Authenticated successfully,<miner_id>,<difficulty>

Notes:
- miner_id is a unique identifier for this connection.
- difficulty is the starting difficulty for shares.

2. Job Handling

2.1 Requesting a job (optional)
Miner -> Server: JOB
Server -> Miner: OK,<job_id>,<miner_hash>,<difficulty>

Notes:
- miner_hash = last_hash ^ miner_id ^ rig_id

2.2 Job push (NEW_JOB)
Server -> Miner (broadcast): NEW_JOB,<job_id>,<miner_hash>,<difficulty>

Notes:
- Miner immediately switches to the new job.
- miner_hash = last_hash ^ miner_id ^ rig_id

3. Share Submission
Miner -> Server: SUBMIT,<job_id>,<extra_nonce2>,<nonce>,<hashrate>
Server -> Miner:
  - Low difficulty share: BAD,Low difficulty share
  - Invalid nonce: BAD,Invalid nonce
  - Other invalid reason: BAD,<optional message>
  - Valid share: GOOD
  - Block found: BLOCK,<block_height>

Notes:
- extra_nonce2 is miner-generated and combined with extra_nonce1.
- difficulty is used to validate the share against miner-assigned difficulty.

4. Difficulty Adjustment
Server -> Miner: SET_DIFF,<difficulty>

- Allows dynamic adjustment of miner difficulty.

5. Disconnect
Miner -> Server: LEAVE
Server -> Miner: OK

Notes:
This version keeps simple CSV-style messages, 128-byte fixed size, and adds:
- Job push (NEW_JOB)
- ExtraNonce collision avoidance
- Dynamic difficulty adjustment
- XOR’d miner_hash for per-miner job uniqueness
'''

class ProtocolHandler:
    def __init__(self, address: str = "0.0.0.0", port: int = 2001, min_version: tuple = (0, 0.0), default_difficulty = 2**24) -> None:
        self.listener = sock.socket(sock.AF_INET, sock.SOCK_STREAM)
        self.address = address
        self.port = port
        self.minimum_version = min_version
        self.default_difficulty = default_difficulty
        self.request_handlers = {
            "AUTH": self.handle_auth
        }
        self.connections = {}

        self.listener.bind((self.address, self.port))
    def wait_for_connection(self) -> tuple[sock.socket, tuple[str, int]]:
        '''
        Waits for a connection request (essentially self.listener.accept())
        Returns:
            Tuple containing the socket that was created after accepting the
            connection, and the address of the source of the request.
        '''
        return self.listener.accept()
    # <wallet>,<rig_name>,<software_name>,<protocol_version>
    def handle_auth(self, sender: sock.socket, wallet: str, rig_name: str, software_name: str, protocol_version: str) -> None:
        '''
        Handles AUTH MSSR request.
        '''
        split_protocol = protocol_version.split(".")
        protocol_version_as_tuple = (int(split_protocol[0]), int(split_protocol[1]))
        if ((protocol_version_as_tuple[0] < self.minimum_version[0]) # If major version is smaller
        or ((protocol_version_as_tuple[0] == self.minimum_version[0]) and (protocol_version_as_tuple[1] < self.minimum_version[1]))): # Or major is the same but minor is smaller
            sender.send("FAIL,1,Your protocol version is too old.")
            return
        
        #TODO: implement wallet checking
        wallet_valid = True
        if not wallet_valid:
            sender.send("FAIL,2,Your wallet is invalid.")
            return
    
        #TODO: implement rig name checking
        connection_identifier = f"{wallet}:{rig_name}"
        rig_invalid = connection_identifier in self.connections.keys()
        if rig_invalid:
            sender.send("FAIL,4,Your rig name is not unique for this wallet.".encode("utf-8"))
            return
        
        
        self.connections[connection_identifier] = AuthenticatedClient(wallet, rig_name, software_name)

        miner_id: str = self.connections[connection_identifier].generate_id()
        sender.send(f"OK,0,Authenticated successfully,{miner_id},{self.default_difficulty}")
        return

class AuthenticatedClient(object):
    def __init__(self, wallet: str, rig_name: str, software_name: str) -> None:
        self.wallet = wallet
        self.rig_name = rig_name
        self.software_name = software_name
    def generate_id(self) -> str:
        return hashlib.sha256(f"{self.wallet}:{self.rig_name}").hexdigest()
    def generate_job_hash(self, last_hash: int) -> int:
        return 0 # TODO