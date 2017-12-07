Methods Object 'all'
	assign: name value: val
		^ name assign: val
|
	== aValue
		^ <21 self aValue>
|
	~~ aValue
		^ (self == aValue) not
|
	~= aValue
		^ self ~~ aValue
|
	= aValue
		^ self == aValue
|
	asString
		^ self printString
|
	basicAt: index
		^ <25 self index>
|
	basicAt: index put: value
		^ <31 self index value>
|
	basicSize
		^ <12 self>
|
	class
		^ <11 self>
|
	copy
		^ self shallowCopy
|
	deepCopy	| newObj |
		newObj <- self class new.
		(1 to: self basicSize) do: 
			[:i | newObj basicAt: i put: (self basicAt: i) copy].
		^ newObj
|
	display
		('(Class ', self class, ') ' , self printString ) print
|
	hash
		^ <13 self>
|
	isMemberOf: aClass
		^ self class == aClass
|
	isNil
		^ false
|
	isKindOf: aClass
		self class upSuperclassChain:
			[:x | (x == aClass) ifTrue: [ ^ true ] ].
		^ false
|
	new
		" default initialization protocol"
		^ self
|
	notNil
		^ true
|
	print
		self printString print 
|
	printString
		^ self class printString
|
	respondsTo: message
		self class upSuperclassChain: 
			[:c | (c methodNamed: message) notNil
					ifTrue: [ ^ true ]].
		^ false
|
	shallowCopy	| newObj |
		newObj <- self class new.
		(1 to: self basicSize) do: 
			[:i | newObj basicAt: i put: (self basicAt: i) ].
		^ newObj
|
	message: m notRecognizedWithArguments: a
		^ smalltalk error: 'not recognized ', (self class printString),
			' ', (m printString)
|
	isNumber
		^ false
|
	isFloat
		^ false
|
	isFraction
		^ false
|
	isInteger
		^ false
|
	isLongInteger
		^ false
|
	isShortInteger
		^ false
]