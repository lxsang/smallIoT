* this should be fix in the compiler, the text variable isnt needed
*Class Method Object text message bytecodes literals stackSize temporarySize class

Methods Method 'all'
	name
		^ message
|
	message: aSymbol
		message <- aSymbol
|
	printString
		^ message asString
|
	text
		^ (text notNil)
			ifTrue: [ text ]
			ifFalse: [ 'text not saved']
|
	text: aString
			text <- aString
|
	signature
		^ class asString,'>>', message asString
|
	display
		('Method ', message) print.
		'bytecodes' print.
		'literals' print.
		literals print.
		bytecodes class print.
		bytecodes do: [:x |
			(x printString, ' ', (x quo: 16), ' ', (x rem: 16))
				print ]
|
	executeWith: arguments
		^ ( Context new ; method: self ; 
			temporaries: ( Array new: temporarySize) ;
			arguments: arguments )
		   returnToBlock: 1
]