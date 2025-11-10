import csv
with open('msg.csv', 'r') as input:
	reader = csv.reader(input)
	header = next(reader)
	for column in range(1, len(header)):
		name = 'msg/{}.po'.format(header[column])
		print(name)
		with open(name, 'w', encoding = "utf-8") as output:
			print('msgid ""', file = output)
			print('msgstr ""', file = output)
			print('"Content-Type: text/plain; charset=UTF-8\\n"', file = output)
			print('"Content-Transfer-Encoding: 8bit\\n"', file = output)
			for line in reader:
				if len(line[column]) > 0:
					print('msgid "{}"'.format(line[0]), file = output)
					print('msgstr "{}"'.format(line[column]), file = output)
