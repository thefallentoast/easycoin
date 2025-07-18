

def get_sensitive_info(where):
    if where == "user": 
        return {
            "host": "localhost",
            "user": "root",
            "password": "",
            "database": "USERS"
        } # Set YOUR information here!
    elif where == "chain":
        return {
            "host": "localhost",
            "user": "root",
            "password": "",
            "database": "CHAIN"
        }