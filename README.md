### OTP - One-Time Pad Encryption

## About this Project
This was an assignment for CS344 - Operating Systems I at Oregon State University. I did not come up with the idea for this project or write the specifications; I only wrote the code to execute it.

### Important
If you are a current student and have stumbled upon this project, please act ethically in accordance with OSU's [code of conduct](https://studentlife.oregonstate.edu/sites/studentlife.oregonstate.edu/files/code-of-student-conduct-102218.pdf). Do not use this project to inform your own work or copy any of the code. Completing this project on your own-- _without_ looking at other solutions-- will be educational and rewarding.

### The Assignment
To the best of my knowledge, this assignment was designed by the instructor, Benjamin Brewster. The assignment provides students the opportunity to practice socket programming and multiprocessing using C. 

The theme for the assignment is the [One-Time Pad](https://en.wikipedia.org/wiki/One-time_pad) encryption technique. In OTP encryption, a plaintext message is combined via modular addition with a key of randomly generated characters that is at least as long as the original message. Anyone with the key can then use it to decrypt the message. In theory, as long as only the encrypter and the trusted decrypter(s) have the key and the key is (A) truly random and (B) never reused, the code cannot be broken by an interceptor.

### Constraints on Input
To simplify the project and allow students to focus on the more interesting tasks of socket-based IPC and multiprocessing, input plaintext file contents were restricted to the ASCII characters A-Z and space. The client programs will print a message to stderr and exit if given files with unacceptable characters.

Plaintext files can be up to 70,000 characters long. 

### Components
_Note: I'm not responsible for the project structure. The assignment requested five programs: one key generator, an encryption daemon, an encryption client, a decryption daemon, and a decryption client._

#### keygen

Usage:
```
$ keygen [length]
```

The program keygen returns the requested number of pseudorandom ASCII characters. What is done with the key is up to the user, but the easiest option is to redirect the program output to a file like so (using Bash):
```
$ keygen [length] > [key file]
```

#### otp_enc_d

Usage: 
```
$ otp_enc_d [port A] &
```
This starts the encryption daemon as a background process listening on [port A]. 

When a client connects to its port, otp_enc_d accepts a message and confirms that it is valid for encryption. If it is an invalid request, it will drop the connection and continue listening; if it is a valid request, it will ask the client for a key. When it receives the key, it uses it to encrypt the first message and sends the results to the client. 

The daemon can handle concurrent connections.

#### otp_enc

Usage: 
```
$ otp_enc [plain text file] [key file] [port A] > [ciphertext file]
```

This starts the encryption client as a foreground process and redirects the output to a file. The client sends the contents of the plaintext file and key file to the daemon at the specified port. The daemon returns the encrypted message, which the client prints to stdout.

#### otp_dec_d

Usage:
```
$ otp_dec_d [port B] &
```
This starts the decryption daemon as a background process listening on [port B].

When a client connects to its port, otp_dec_d accepts a message and confirms that it is valid for decryption. If it is an invalid request, it will drop the connection and continue listening; if it is a valid request, it will ask the client for a key. When it receives the key, it uses it to decrypt the first message and sends the results to the client. 

The daemon can handle concurrent connections.

#### otp_dec

Usage:
```
$ otp_dec [ciphertext file] [key file] [port B] > [result file]
```

This starts the encryption client as a foreground process and redirects the output to a file. The client sends the contents of the ciphertext file and key file to the daemon at the specified port. The daemon returns the decrypted message, which the client prints to stdout.

### Compilation

I've included a Bash compilation script.

Usage:
```
$ bash compileall
```

### Sample Execution

```
$ bash compileall
$ otp_enc_d 57001 &
$ opt_dec_d 57002 &
$ cat plaintext/sample1
MY HOVERCRAFT IS FULL OF EELS
$ keygen 1024 > keyfile
$ otp_enc plaintext/sample1 keyfile 57001 > cipher1
$ cat cipher1
NQIXXHLGYDTVBHMQROFANHTJYI YX
$ otp_dec cipher1 keyfile 57002 > sample1_dec
$ cat sample1_dec
MY HOVERCRAFT IS FULL OF EELS
$ cmp plaintext/sample1 sample1_dec
$ echo $?
0
```
