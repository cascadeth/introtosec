#import hmac, base64, struct, hashlib, time

import onetimepass as totp
import sys
import os.path
import time

SECRET = "SooperSecretCode"
TOKEN = -1
PREV = -1

print "\n\nStarting loop to show phase difference\nWatch the G. Authenticator app...\nCtrl+C to exit\n\n"

# Debugging loop to show time offset
if DEBUG:

    while True:

        TOKEN = totp.get_totp(SECRET)
        if TOKEN != PREV:
            PREV = TOKEN
            print "New Token here:", TOKEN
            time.sleep(15)
            print "App Gen - Check"


print "\nGreat! Now go do something useful..."
exit()
