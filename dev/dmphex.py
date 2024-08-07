import sys

def hex_to_string(hex_str):
    # Remove the '\x' from the hex string
    hex_str = hex_str.replace('\\x', '')
    
    # Split into pairs of characters
    pairs = [hex_str[i:i+2] for i in range(0, len(hex_str), 2)]
    
    # Convert each pair from hex to character
    chars = ''.join([chr(int(pair, 16)) for pair in pairs])
    
    return chars

if __name__ == "__main__":
    if len(sys.argv) !=2:
        print("enter hex string!!")
        sys.exit(1)
    hex_string = sys.argv[1]
#"\\x48\\x65\\x6c\\x6c\\x6f\\x20\\x57\\x6f\\x72\\x6c\\x64"
    result = hex_to_string(hex_string)
    print(result)
