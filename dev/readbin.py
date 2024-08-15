try:
   with open("app.bin", "rb") as f:
      byte = f.read(1)
      while byte:
       #  if(byte != b'\x00'):
         print(f"{byte.hex()}", end=" ")
         byte = f.read(1)
   print()
except FileNotFoundError:
   print("nope")
