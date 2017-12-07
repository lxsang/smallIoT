*Class Symbol Object

Methods Symbol 'all'
    	apply: args
		^ self apply: args ifError: [ 'does not apply' ]
|
    	apply: args ifError: aBlock
		^ smalltalk perform: self withArguments: args ifError: aBlock
|
	assign: value
		<27 self value>. ^ value
|
	asString
		" catenation makes string and copy automatically "
		^ <24 self ''>
|
	copy
		^ self
|
	printString
		^ '#' , self asString
|
	respondsTo
		^ classes inject: Set new
			into: [:x :y | ((y methodNamed: self) notNil)
						ifTrue: [ x add: y]. x]
|
	value
		^ <87 self>
]
