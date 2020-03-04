import json
import subprocess

params = {
    'pan-id' : '0xefff',
    'device-mac' : '21:3b:5c:8a:09:2a:9f:00',
    'wpan-interface' : 'wpan0',
    'device-role' : 'router'
}

jsonString = json.dumps(params)
print(jsonString)

p = subprocess.Popen(['../../bin/zdo_wpan_start_zr'], stdin=subprocess.PIPE)
ret = p.communicate(jsonString.encode('latin1'))

