import csv
import sys

q1 = 0
q2 = 0
with open(sys.argv[1], 'r') as i:
    reader = csv.reader(i, delimiter=',')

    k = 1
    d = {}
    g = {}
    q1 = 0
    for row in reader:
      v1 = (row[0], row[1])
      v2 = (row[2], row[3])

      if v1 not in d:
        d[v1] = k
        k = k + 1

      if v2 not in d:
        d[v2] = k
        k = k + 1

      c1 = d[v1]
      c2 = d[v2]
      if (c1, c2) in g: 
          continue
      g[(c1, c2)] = True
      q1 += 1
    q2 = k - 1


with open(sys.argv[1], 'r') as i, open(sys.argv[2], 'w') as o, open('dict.txt', 'w') as f:
    reader = csv.reader(i, delimiter=',')
    o.write("%%MatrixMarket matrix coordinate pattern general\n" +
            "%%GraphBLAS type bool\n")
    o.write(f"{q2} {q2} {q1}\n")
    k = 1
    d = {}
    g = {}
    for row in reader:
        v1 = (row[0], row[1])
        v2 = (row[2], row[3])

        if v1 not in d:
            d[v1] = k
            f.write(f'{v1[0]},{v1[1]} {k}\n')
            k = k + 1

        if v2 not in d:
            d[v2] = k
            f.write(f'{v2[0]},{v2[1]} {k}\n')
            k = k + 1

        c1 = d[v1]
        c2 = d[v2]
        if (c1, c2) in g: 
            continue
        g[(c1, c2)] = True
        o.write(f'{c1} {c2}\n')

