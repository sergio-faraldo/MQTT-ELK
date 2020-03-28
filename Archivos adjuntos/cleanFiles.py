import json
from pathlib import Path
import os
from datetime import datetime, timedelta
import sys
with open(sys.argv[1], 'r') as f:
	data = json.load(f)
data2 = data.copy()
for entry in data:
	if(os.path.exists(entry['source'])):
		if(datetime.now() - datetime.fromtimestamp(Path(entry['source']).stat().st_ctime) > timedelta(days=2)):
			if(Path(entry['source']).stat().st_size==entry['offset']):
				os.remove(entry['source'])
				data2.remove(entry)
	else:
		data2.remove(entry)
with open(sys.argv[1], "w") as f:
	f.write(json.dumps(data2).replace(" ",""))
