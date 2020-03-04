import json
import subprocess

params = {
    'pan-id' : '0xefff',
    'device-mac' : '21:3b:5c:8a:09:2a:9f:00',
    'wpan-interface' : 'wpan0',
    'device-role' : 'coordinator'
}

jsonString = json.dumps(params)
print(jsonString)

p = subprocess.run(['./test_settings_from_pipe'], input=jsonString.encode('latin1'))
print('process returned', p.returncode)

