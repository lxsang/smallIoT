*Class Class Object name instanceSize methods superClass variables

Methods Class 'creation'
	new		| newObject |
		newObject <- self new: instanceSize.
		^ (self == Class)
			ifTrue: [ newObject initialize ]
			ifFalse: [ newObject new ]
|
	new: size	" hack out block the right size and class "
			"create a new block, set its class"
			self = ByteArray ifTrue:[^ <22 <59 size> ByteArray>].
			^ < 22 < 58 size > self >
|
	addSubClass: aSymbol instanceVariableNames: aString	| newClass |
		newClass <- Class new; name: aSymbol; superClass: self;
				variables: 
				  (aString words: [:x | x isAlphabetic ]).
		aSymbol assign: newClass.
		classes at: aSymbol put: newClass
|
	initialize
		superClass <- Object.
		instanceSize <- 0.
		methods <- Dictionary new
|
	methods
		^ methods
|
	methodNamed: name
		(methods includesKey: name)
			ifTrue: [ ^ methods at: name ].
		(superClass notNil)
			ifTrue: [ ^ superClass methodNamed: name ].
		^ nil
|
	name
		^ name
|
	name: aString
		name <- aString
|
	instanceSize
		^ instanceSize
|
	printString
		^ name asString
|
	respondsTo	| theSet |
		theSet <- Dictionary new.
		self upSuperclassChain: 
			[:x | theSet addAll: x methods ].
		^ theSet
|
	subClasses
		^ classes inject: List new
			into: [:x :y | (y superClass == self)
						ifTrue: [ x add: y]. x ]
|
	superClass
		^ superClass
|
	superClass: aClass
		superClass <- aClass
|
	upSuperclassChain: aBlock
		aBlock value: self.
		(superClass notNil)
			ifTrue: [ superClass upSuperclassChain: aBlock ]
|
	variables
		^ variables
|
	variables: nameArray
		variables <- nameArray.
		instanceSize <- superClass instanceSize + nameArray size
|
	watch: name	| m |
		m <- self methodNamed: name.
		(m notNil) 
			ifTrue: [ ^ m watch: 
				[:a | ('executing ', name) print. a print] ]
			ifFalse: [ ^ 'no such method' ]
|
	display
		('Class name: ', name asString)  print.
		(superClass notNil)
			ifTrue: [ ('Superclass: ', superClass ) print ].
		'Instance Variables:' print.
		variables isNil
			ifTrue: [ 'no instance variables ' print ]
			ifFalse: [ variables display ].
		'Subclasses: ' print.
		self subClasses display
]