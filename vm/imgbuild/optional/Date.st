Class Date Object d m y h mi s stamp
Methods Date 'all'

year
    ^y

|

new
	^ (< 22 <153 0> (self class) >)

|
stamp
    ^ stamp

|
printString
    ^ (d printString),'/',
        (m printString),'/',
        (y printString),' ',
        (h printString),':',
        (mi printString),':',
        (s printString)
        
|
day
    ^d

|

second
    ^s

|

month
    ^m

|

minute
    ^mi

|

hour
    ^h

]
