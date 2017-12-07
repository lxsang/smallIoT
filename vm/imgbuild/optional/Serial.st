Class Serial Object
Methods Serial 'all'
	getChar
		^ self getByte asCharacter
| getByte
	^ (<130>)
|
	writeByte:aValue
		<131 aValue>
|
	writeChar:aChar
		self writeByte: aChar value
|
	read:length
		^ (<132 length>)
|
	write:aByteArray
		<133 aByteArray (aByteArray size)>
|
	readFile 
		^ <134>
]

Methods ByteArray 'all'
	asInteger
		self size = 4 ifFalse:[
			^ self error:'ByteArray size != 4'].
		^ (<14 self>)
]

Methods Integer 'all'
	asByteArray |arr|
		arr <- ByteArray new:4.
		arr at:1 put: (self bitAnd:255).
		arr at:2 put: ((self bitShift:-8) bitAnd:255).
		arr at:3 put: ((self bitShift:-16) bitAnd:255).
		arr at:4 put: (self bitShift:-24).
		^arr
]