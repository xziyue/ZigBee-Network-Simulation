import json
import subprocess

params = {
    'pan-id' : '0xbeef',
    'device-mac' : '21:3b:5c:8a:09:2a:9f:00',
    'wpan-interface' : 'wpan0',
    'device-role' : 'coordinator',
    'from-ip' : '127.0.0.1',
    'to-ip' : '127.0.0.1',
    'udp-sport' : '60009',
    'udp-dport' : '60010'
}

jsonString = json.dumps(params)
print(jsonString)


p = subprocess.Popen(['../../bin/zdo_wpan_start_zc'], stdin=subprocess.PIPE)
ret = p.communicate(jsonString.encode('latin1'))

