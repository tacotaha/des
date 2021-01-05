# des

https://csrc.nist.gov/csrc/media/publications/fips/46/3/archive/1999-10-25/documents/fips46-3.pdf


## Usage: 

```./des -[d/e] <key> <infile> <outfile>```

  where key is a file containing a 64-bit secret key
 
 ## Example:
  * encryption: ```./des -e key.txt plaintext.txt ciphertext.bin```
  * decryption:``` ./des -d key.txt ciphertext.bin plaintext.txt```
