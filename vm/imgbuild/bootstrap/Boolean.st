*Class Boolean Object
*Class    True Boolean
*Class    False Boolean

Methods Boolean 'all'
	ifTrue: trueBlock
		^ self ifTrue: trueBlock ifFalse: []
|
	ifFalse: falseBlock
		^ self ifTrue: [] ifFalse: falseBlock
|
	ifFalse: falseBlock ifTrue: trueBlock
		^ self ifTrue: trueBlock
			ifFalse: falseBlock
|
	and: aBlock
		^ self ifTrue: aBlock ifFalse: [ false ]
|
	| value
		 ^ self or:[aBool]
|
	or: aBlock
		^ self ifTrue: [ true ] ifFalse: aBlock
]

Methods False 'all'
	ifTrue: trueBlock ifFalse: falseBlock
		^ falseBlock value
|
	not
		^ true
|
	xor: aBoolean
		^ aBoolean
|
	printString
		^ 'false'
]

Methods True 'all'
	ifTrue: trueBlock ifFalse: falseBlock
		^ trueBlock value
|
	not
		^ false
|
	xor: aBoolean
		^ aBoolean not
|
	printString
		^ 'true'
]