import json
import subprocess

params = {
    'device-mac' : '4f:0a:fc:8a:82:2a:3c:8d',
    'wpan-interface' : 'wpan1',
    'device-role' : 'router',
    'from-ip' : '127.0.0.1',
    'to-ip' : '127.0.0.1',
    'udp-sport' : '60010',
    'udp-dport' : '60009'
}

jsonString = json.dumps(params)
print(jsonString)


p = subprocess.Popen(['../../bin/zdo_wpan_start_zr'], stdin=subprocess.PIPE)
ret = p.communicate(jsonString.encode('latin1'))

