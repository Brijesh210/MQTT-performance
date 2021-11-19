import string
import xlsxwriter

file1 = open('data\\data2.txt', 'r')
Lines = file1.readlines()

workbook = xlsxwriter.Workbook("data\\4-3-2.xlsx")
worksheet = workbook.add_worksheet()

worksheet.write_row(0, 1, string.ascii_uppercase)
worksheet.write_column(1, 0, range(1, 51))

for i, line in enumerate(Lines):
    columns = line[12:-3].split(',')
    intColumns = list(map(int, columns))
    worksheet.write_column(1, i + 1, intColumns)

workbook.close()
print("Done")
