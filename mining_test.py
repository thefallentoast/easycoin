from hash import hash

def mine(difficulty: int, previous_hash: str):
    # Difficulty is expected in form 0xFFFF, 0xF, 0xFFFFFFFF etc
    # Previous hash is expected in form '0xAE0B21CC' (basically output of hex(hash))
    h = 0xDEADBEEF
    nonce = 0
    while h > difficulty:
        print(f"Attempt {nonce}")
        h = hash(previous_hash[2:] + str(nonce))
        nonce += 1
    
    print(f"Found: {hex(h)} || Difficulty: {hex(difficulty)} || Nonce: {nonce}")
    
print(mine(0x000000FF, '0x000000CC'))