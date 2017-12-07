Class Parser Object text index tokenType token argNames tempNames instNames maxTemps errBlock lineNum
Class ParserNode Object lineNum
Class Encoder Object name byteCodes index literals stackSize maxStack
Class BodyNode ParserNode statements
Class ReturnNode ParserNode expression
Class AssignNode ParserNode target expression
Class LiteralNode ParserNode value
Class ArgumentNode ParserNode position
Class TemporaryNode ParserNode position
Class InstNode ParserNode position
Class PrimitiveNode ParserNode number arguments
Class BlockNode ParserNode statements temporaryLocation temporaryCount
Class CascadeNode ParserNode head list
Class MessageNode ParserNode receiver name arguments

* --------------parser methods-----------------
Methods Parser 'all'
text: aString instanceVars: anArray
	text <- aString.
	index <- 1.
	lineNum <- 1.
	argNames <- Array new: 1.
	argNames at: 1 put: #self.
	anArray isNil ifTrue:[instNames <- Array new:0] ifFalse:[
		instNames <- anArray].
	tempNames <- Array new: 0.
	maxTemps <- 0
|
parse: c with: encoderClass	| encoder meth name |
	" note -- must call text:instanceVars: first "
	errBlock <- [ ^ nil ].
	self nextLex.
	encoder <- encoderClass new.
	name <- self readMethodName.
	(c ~= UndefinedObject) ifTrue: [
		'Compiling ' print. c print. ':' print.
		name print. '... ' print
	].
	encoder name: name.
	self readMethodVariables.
	self readBody compile: encoder block: false.
	meth <- encoder method: maxTemps class: c text: text.
	"meth args: argNames inst: instNames temp: tempNames."
	^ meth
|
parse: c
	^ self parse: c with: Encoder
|
error: aString
	('Compile error near line ',
	(lineNum printString),
	': ',
	aString) print.
	errBlock value
|
currentChar
	^ text at: index ifAbsent: [ 256 asCharacter ]
|
nextChar
    | c |
    c <- self currentChar.
  	(c isNewLine) ifTrue: [ lineNum <- lineNum + 1 ].
  	index <- index + 1.
    (c isCr) ifTrue: [ ^self nextChar ].
  	^self currentChar
|
nextLex	
	self skipBlanks.
	tokenType <- self currentChar.
	tokenType isEOF   " end of input "
		ifTrue: [ tokenType <- $  . token <- nil. ^ nil ].
	tokenType isDigit ifTrue: [ ^ self lexNumber"lexInteger" ].
	tokenType isAlphabetic ifTrue: [ ^ self lexAlnum ].
	^ self lexBinary
|
skipBlanks  | cc |
	cc <- self currentChar.
	[ cc isBlank ] whileTrue: [ cc <- self nextChar ].
	( cc = $" ) ifTrue: [ self skipComment ]
|
skipComment  | cc |
	[ cc <- self nextChar.
	  cc isEOF ifTrue: [ ^ self error: 'unterminated comment'].
	  cc ~= $" ] whileTrue: [ nil ].
	self nextChar. self skipBlanks
|
lexNumber	| start cc float|
	start <- index.
	float <- false.
	[ self nextChar isDigit ] whileTrue.
	cc <- self currentChar.
	cc = $. ifTrue:[ "possible float"
		cc <- self nextChar.
		cc isDigit ifTrue:[
			float<-true.
			[cc <- self nextChar.cc isDigit] whileTrue
		] ifFalse:[index <- index - 1.cc <- self currentChar]
	].
    ((cc = $e) or:[cc = $E]) ifTrue:[
		float <- true.
    	cc <- self nextChar.
     	((cc isDigit or:[cc = $+]) or:[cc = $-]) ifFalse:[
     		^self error:'expect +/-/digit'].
        [cc <- self nextChar. cc isDigit] whileTrue
    ].
	token <- text copyFrom: start to: index - 1.
	token <- float ifTrue:[token asFloat] ifFalse:[token asInteger]
|
lexAlnum | cc start |
	start <- index.
	[ ((cc <- self nextChar) isAlphaNumeric) or: [ 
	cc = $: and:[ self nextChar isAlphabetic ] ] ]
			whileTrue: [ nil ].
		" add any trailing colons "
	token <- text copyFrom: start to: index - 1.
|
lexBinary	| c d |
	c <- self currentChar.
	token <- c asString.
	d <- self nextChar.
	(self charIsSyntax: c) ifTrue: [ ^ token ].
	(((d isBlank
		or: [ d isDigit])
		or: [ d isAlphabetic ])
		or: [ self charIsSyntax: d])
			ifTrue: [ ^ token ].
	token <- (token,(d asString)).
	self nextChar
|
charIsSyntax: c
	^ ('.()[]#^$;{}' includes: c) or: [ c = $' ]
|
readMethodName   | name |
	self tokenIsName	" unary method "
		ifTrue: [ name <- token. self nextLex. ^ name ].
	self tokenIsBinary	" binary method "
		ifTrue: [ name <- token. self nextLex.
			self tokenIsName
				ifFalse: [ self error: 'missing argument'].
				self addArgName: token asSymbol.
				self nextLex. ^ name ].
	self tokenIsKeyword
		ifFalse: [ self error: 'invalid method header:',token].
	name <- ''.
	[ self tokenIsKeyword ]
		whileTrue: [ name <- (name,token). self nextLex.
			self tokenIsName
				ifFalse: [ self error: 'missing argument'].
				self addArgName: token asSymbol.
				self nextLex ].
	^ name
|
addArgName: name
	((instNames includes: name)
		or: [ argNames includes: name ])
		ifTrue: [ self error: 'doubly defined argument name: ' +
			name asString].
	argNames <- argNames with: name
|
tokenIsName
	tokenType isAlphabetic ifFalse: [ ^ false ].
	^ (token at: token size) isAlphaNumeric
|
tokenIsKeyword
	tokenType isAlphabetic ifFalse: [ ^ false ].
	^ (token at: token size) = $:
|
tokenIsBinary
	(((token isNil
		or: [ self tokenIsName])
		or: [ self tokenIsKeyword])
		or: [ self charIsSyntax: tokenType ]) ifTrue: [ ^ false ].
	^ true
|
readMethodVariables
	tokenType = $| ifFalse: [ ^ nil ].
	self nextLex.
	[ self tokenIsName ]
		whileTrue: [ self addTempName: token asSymbol. self nextLex ].
	tokenType = $|
		ifTrue: [ self nextLex ]
		ifFalse: [ self error: 'illegal method variable declaration']
|
addTempName: name
	(((argNames includes: name)
		or: [ instNames includes: name ] )
		or: [ tempNames includes: name ] )
		ifTrue: [ self error: 'doubly defined name '].
	tempNames <- tempNames with: name.
	maxTemps <- maxTemps max: tempNames size
|
readBody | lnum |
	lnum <- lineNum.
	^ ((BodyNode new) at:lnum) statements: self readStatementList
|
readStatementList   | list |
	list <- List new.
	[ list add: self readStatement.
	  tokenType notNil and: [ tokenType = $. ] ]
		whileTrue: [ self nextLex.
			(token isNil or: [ tokenType = $] ] )
				ifTrue: [ ^ list ] ].
	^ list
|
readStatement | lnum |
	tokenType = $^
		ifTrue: [ lnum <- lineNum. self nextLex.
			^ ((ReturnNode new) at: lnum)
				expression: self readExpression ].
	^ self readExpression
|
readExpression   | node lnum |
	self tokenIsName ifFalse: [ ^ self readCascade: self readTerm ].
	node <- self nameNode: token asSymbol. self nextLex.
	self tokenIsArrow
		ifTrue: [ node assignable
				ifFalse: [ self error: 'illegal assignment'].
			lnum <- lineNum.
			self nextLex.
			^ ((AssignNode new) at: lnum) target:
				node expression: self readExpression ].
	^ self readCascade: node
|
tokenIsArrow
	(token isKindOf: String) ifFalse: [ ^ false ].
	^ token = '<-' or:[token = ':=']
|
readTerm   | node lnum |
	token isNil
		ifTrue: [ self error: 'unexpected end of input' ].
	tokenType = $(
		ifTrue: [ self nextLex. node <- self readExpression.
			tokenType = $)
				ifFalse: [ self error: 'unbalanced parenthesis' ].
			self nextLex. ^ node ].
	tokenType = $[ ifTrue: [ ^ self readBlock ].
	tokenType = $< ifTrue: [ ^ self readPrimitive ].
	tokenType = ${ ifTrue:[^self readDynamicArray ].
	self tokenIsName
		ifTrue: [ node <- self nameNode: token asSymbol.
			self nextLex. ^ node ].
	lnum <- lineNum.
	^ ((LiteralNode new) at: lnum) value: self readLiteral
|
nameNode: name
	" make a new name node "
	name == #super
		ifTrue: [ ^ ((ArgumentNode new) at: lineNum) position: 0 ].
	(1 to: tempNames size) do: [:i |
		(name == (tempNames at: i))
			ifTrue: [ ^ ((TemporaryNode new) at: lineNum)
				position: i ] ].
	(1 to: argNames size) do: [:i |
		(name == (argNames at: i))
			ifTrue: [ ^ ((ArgumentNode new) at: lineNum) position: i ] ].
	(1 to: instNames size) do: [:i |
		(name == (instNames at: i))
			ifTrue: [ ^ ((InstNode new) at: lineNum) position: i ] ].
	^ ((LiteralNode new) at: lineNum)
		value: (<87 name>)
|
readDynamicArray   | msg|
	msg <- ((MessageNode new) at: lineNum) 
	    receiver:(((LiteralNode new) at: lineNum) value:Array)
	    name: #new: 
	    arguments: ((Array new:0) with:(((LiteralNode new) at: lineNum) value:0)).
	self nextLex.
	[   
	    tokenType = $} ifFalse:[
	        msg <- ((MessageNode new) at: lineNum) receiver:msg 
	            name: #with: arguments: ((Array new:0) with:self readStatement)
	   ].
	    tokenType notNil and: [ tokenType = $. ] ]
		whileTrue:[self nextLex].
		(tokenType = $})
				ifTrue: [ self nextLex. ^ msg ]
		        ifFalse:[^ self error:'Expected }']
|
readLiteral   | node |
	tokenType = $$
		ifTrue: [ node <- self currentChar.
			self nextChar. self nextLex. ^ node ].
	tokenType isDigit
		ifTrue: [ ^ self readNumber ].
	token = '-'
		ifTrue: [ self nextLex. ^ self readNumber negated ].
	tokenType = $'
		ifTrue: [ ^ self readString ].
	tokenType = $#
		ifTrue: [ ^ self readSymbol ].
	self error: 'invalid literal: ' + token
|
readNumber  | value |
	value <- token.
	value isNil ifTrue: [ self error: 'number expected' ].
	self nextLex.
	^ value
|
readString  | first last cc |
	first <- index.
	[ cc <- self currentChar.
	  cc isNil ifTrue: [ self error: 'unterminated string constant'].
	  cc ~= $' ] whileTrue: [ index <- index + 1 ].
	last <- index - 1.
	self nextChar = $'
		ifTrue: [ self nextChar.
			^ (text copyFrom: first to: index - 2) + self readString ].
	self nextLex.
	^ text copyFrom: first to: last
|
readSymbol   | cc |
	cc <- self currentChar.
	(cc isEOF or: [ cc isBlank])
		ifTrue: [ self error: 'invalid symbol'].
	cc = $( ifTrue: [ ^ self readArray ].
	(self charIsSyntax: cc)
		ifTrue: [ self error: 'invalid symbol'].
	self nextLex.
	cc <- token asSymbol. self nextLex.
	^ cc
|
readArray	| value |
	self nextChar. self nextLex. value <- Array new: 0.
	[ tokenType ~= $) ]
		whileTrue: [ value <- value with: self arrayLiteral ].
	self nextLex.
	^ value
|
arrayLiteral	| node |
	tokenType isAlphabetic
		ifTrue: [ node <- token asSymbol. self nextLex. ^ node ].
	^ self readLiteral
|
readPrimitive  | num args lnum |
	lnum <- lineNum.
	self nextLex.
	num <- self readNumber.
	args <- List new.
	[ tokenType ~= $> ]
		whileTrue: [ args add: self readTerm ].
	self nextLex.
	^ ((PrimitiveNode new) at: lnum) number: num arguments: args
|
readBlock    | stmts saveTemps lnum cnt|
	saveTemps <- tempNames.
	lnum <- lineNum.
	self nextLex.
	cnt <- 0.
	tokenType = $:
		ifTrue: [ cnt <- self readBlockTemporaries ].
	stmts <- self readStatementList.
	tempNames <- saveTemps.
	tokenType = $]
		ifTrue: [ self nextLex.
			^ ((BlockNode new) at: lnum) statements: stmts
				temporaryLocation: saveTemps size temporaryCount:cnt]
		ifFalse: [ self error: 'unterminated block']
|
readBlockTemporaries |cnt|
	cnt <- 0.
	[ tokenType = $: ]
		whileTrue: [ self currentChar isAlphabetic
			ifFalse: [ self error: 'ill formed block argument'].
			self nextLex.
			self tokenIsName
				ifTrue: [ self addTempName: token asSymbol. cnt <- cnt+1]
				ifFalse: [ self error: 'invalid block argument list '].
			self nextLex ].
	tokenType = $|
		ifTrue: [ self nextLex ]
		ifFalse: [ self error: 'invalid block argument list '].
	^ cnt
|
readCascade: base   | node list |
	node <- self keywordContinuation: base.
	tokenType = $;
		ifTrue: [ node <- ((CascadeNode new) at: lineNum) head: node.
			list <- List new.
			[ tokenType = $; ]
				whileTrue: [ self nextLex.
					list add:
						(self keywordContinuation: nil ) ].
			node list: list ].
	^ node
|
keywordContinuation: base  | receiver name args lnum |
	receiver <- self binaryContinuation: base.
	self tokenIsKeyword
		ifFalse: [ ^ receiver ].
	name <- ''.
	args <- List new.
	lnum <- lineNum.
	[ self tokenIsKeyword ]
		whileTrue: [ name <- name + token. self nextLex.
			args add:
				(self binaryContinuation: self readTerm) ].
	^ ((MessageNode new) at: lnum) receiver:
		receiver name: name asSymbol arguments: args
|
binaryContinuation: base | receiver name lnum |
	receiver <- self unaryContinuation: base.
	[ self tokenIsBinary]
		whileTrue: [ lnum <- lineNum.
			name <- token asSymbol. self nextLex.
			receiver <- ((MessageNode new) at: lnum)
				receiver: receiver name: name arguments:
					((List new) add:
						(self unaryContinuation: self readTerm)) ].
	^ receiver
|
unaryContinuation: base | receiver lnum |
	receiver <- base.
	[ self tokenIsName ]
		whileTrue: [ lnum <- lineNum.
			receiver <- ((MessageNode new) at: lnum)
				receiver: receiver name: token asSymbol
					arguments: (List new).
				self nextLex ].
	^ receiver
]

Methods ParserNode
at:l
	lineNum <- l
|
isSuper
	^ false
|
isBlock
	^ false
|
assignable
	^ false
|
compile: encoder
	encoder lineNum: lineNum
]
Methods BodyNode
statements: s
	statements <- s
|
compile: encoder block: inBlock
	super compile: encoder.
	"statements reverseDo:"
	statements do:
		[ :stmt | stmt compile: encoder block: inBlock.
			encoder genHigh: 15 low: 5 " pop "].
	encoder genHigh: 15 low: 1 " return self "
]

Methods ReturnNode
expression: e
	expression <- e
|
compile: encoder block: inBlock
	super compile: encoder.
	expression compile: encoder block: inBlock.
	inBlock
		ifTrue: [ encoder genHigh: 15 low: 3]
		ifFalse: [ encoder genHigh: 15 low: 2 " stack return " ]
]
Methods AssignNode
target: t expression: e
	target <- t.
	expression <- e
|
compile: encoder block: inBlock
	super compile: encoder.
	expression compile: encoder block: inBlock.
	target assign: encoder
]

Methods LiteralNode
value: v
	value <- v
|
compile: encoder block: inBlock
	super compile: encoder.
	value == nil ifTrue: [ ^ encoder genHigh: 5 low: 10 ].
	value == true ifTrue: [ ^ encoder genHigh: 5 low: 11 ].
	value == false ifTrue: [ ^ encoder genHigh: 5 low: 12 ].
	(((value class == Integer) and:
	 [value < 10]) and: [value negative not])
		ifTrue: [ ^ encoder genHigh: 5 low: value ].
	encoder genHigh: 4 low: (encoder genLiteral: value).
]

Methods ArgumentNode
position: p
	position <- p
|
isSuper
	^ position = 0
|
compile: encoder block: inBlock
	super compile: encoder.
	position = 0
		ifTrue: [ encoder genHigh: 2 low: 0 ]
		ifFalse: [ encoder genHigh: 2 low: position - 1 ]
]
Methods TemporaryNode
position: p
	position <- p
|
compile: encoder block: inBlock
	super compile: encoder.
	encoder genHigh: 3 low: position - 1
|
assignable
	^ true
|
assign: encoder
	encoder genHigh: 7 low: position - 1
]

Methods InstNode
position: p
	position <- p
|
compile: encoder block: inBlock
	super compile: encoder.
	encoder genHigh: 1 low: position - 1
|
assign: encoder
	encoder genHigh: 6 low: position - 1
|
assignable
	^ true
]
Methods PrimitiveNode
number: n arguments: a
	number <- n.
	arguments <- a.
|
compile: encoder block: inBlock | argsize |
	argsize <- arguments size.
	super compile: encoder.
	encoder pushArgs: argsize.
	"arguments reverseDo: [ :a | a compile: encoder block: inBlock ]."
	arguments do: [ :a | a compile: encoder block: inBlock ].
	encoder genHigh: 13 low: argsize.
	encoder genCode: number.
	encoder popArgs: argsize
]

Methods BlockNode
statements: s temporaryLocation: t temporaryCount:cnt
	statements <- s.
	temporaryLocation <- t.
	temporaryCount <- cnt
|
compileInLine: encoder block: inBlock
	"statements reverseDo:"
	statements do:
		[ :stmt | stmt compile: encoder block: inBlock.
			encoder genHigh: 15 low: 5 " pop top " ].
	encoder backUp
|
isBlock
	^ true
|
compile: encoder block: inBlock | patchLocation |
	super compile: encoder.
	"encoder genHigh: 12 low: temporaryLocation."
	encoder genHigh: 0 low: 12.
	encoder genHigh: temporaryLocation low: temporaryCount.
	"encoder genHigh: 15 low: 6. branch"
	patchLocation <- encoder genVal: 0.
	self compileInLine: encoder block: true.
	encoder genHigh: 15 low: 2. " return top of stack "
	encoder patch: patchLocation
]
Methods CascadeNode
head: h
	head <- h
|
list: l
	list <- l
|
compile: encoder block: inBlock
	super compile: encoder.
	head compile: encoder block: inBlock.
	"list reverseDo: [ :stmt |"
	list do: [ :stmt |
		encoder genHigh: 15 low: 4. " duplicate "
		stmt compile: encoder block: inBlock.
		encoder genHigh: 15 low: 5 "pop from stack " ]
]

Methods MessageNode 'all'
receiver: r name: n arguments: a
	receiver <- r.
	name <- n.
	arguments <- a
|
compile: encoder block: inBlock
	super compile: encoder.
	receiver isNil
		ifTrue: [ ^ self cascade: encoder block: inBlock ].
	((receiver isBlock and: [ self argumentsAreBlock ])
		and: [name = #whileTrue: or: [ name = #whileFalse ] ] )
		ifTrue: [ ^ self optimizeWhile: encoder block: inBlock ].
	receiver compile: encoder block: inBlock.
	receiver isSuper
		ifTrue: [ ^ self sendToSuper: encoder block: inBlock ].
	name = #isNil ifTrue: [ ^ encoder genHigh: 10 low: 0 ].
	name = #notNil ifTrue: [ ^ encoder genHigh: 10 low: 1 ].
	self compile2: encoder block: inBlock
|
compile2: encoder block: inBlock
	self argumentsAreBlock ifTrue: [
		name = #ifTrue: ifTrue: [ ^ self compile: encoder
				test: 8 constant: 10 block: inBlock ].
		name = #ifFalse: ifTrue: [ ^ self compile: encoder
				test: 7 constant: 10 block: inBlock ].
		name = #and: ifTrue: [ ^ self compile: encoder
				test: 8 constant: 12 block: inBlock ].
		name = #or: ifTrue: [ ^ self compile: encoder
				test: 7 constant: 11 block: inBlock ].
		name = #ifTrue:ifFalse:
			ifTrue: [ ^ self optimizeIf: encoder block: inBlock ]
		].
	self evaluateArguments: encoder block: inBlock.
	name = '<' asSymbol ifTrue: [ ^ encoder genHigh: 11 low: 2].
	name = '<=' asSymbol ifTrue: [ ^ encoder genHigh: 11 low: 4].
	name = '+' asSymbol ifTrue: [ ^ encoder genHigh: 11 low: 0].
	self sendMessage: encoder block: inBlock
|
sendToSuper: encoder block: inBlock
	self evaluateArguments: encoder block: inBlock.
	encoder genHigh: 8 low: 1 + arguments size.
	encoder genHigh: 15 low: 11.
	encoder genCode: (encoder genLiteral: name)
|
cascade: encoder block: inBlock
	self evaluateArguments: encoder block: inBlock.
	self sendMessage: encoder block: inBlock
|
evaluateArguments: encoder block: inBlock
	encoder pushArgs: 1 + arguments size.
	"arguments reverseDo: [ :arg |"
	arguments do: [ :arg |
		arg compile: encoder block: inBlock ]
|
sendMessage: encoder block: inBlock
	encoder popArgs: arguments size.
		" mark arguments, then send message "
	encoder genHigh: 8 low: 1 + arguments size.
	encoder genHigh: 9 low: (encoder genLiteral: name).
|
argumentsAreBlock
	arguments do: [ :arg | arg isBlock ifFalse: [ ^ false ]].
	^ true
|
optimizeWhile: encoder block: inBlock | start save |
	start <- encoder currentLocation + 1.
	receiver compileInLine: encoder block: inBlock.
	name = #whileTrue:	" branch if false/true "
		ifTrue: [ encoder genHigh: 15 low: 8 ]
		ifFalse: [ encoder genHigh: 15 low: 7 ].
	save <- encoder genVal: 0.
	arguments first compileInLine: encoder block: inBlock.
	encoder genHigh: 15 low: 5. " pop from stack "
	encoder genHigh: 15 low: 6. " branch "
	encoder genVal: start. " branch target "
	encoder patch: save.
	encoder genHigh: 5 low: 10  " push nil "
|
compile: encoder test: t constant: c block: inBlock | save ssave |
	super compile: encoder.
	encoder genHigh: 15 low: t.  " branch test "
	save <- encoder genVal: 0.
	arguments first compileInLine: encoder block: inBlock.
	encoder genHigh: 15 low: 6.  " branch "
	ssave <- encoder genVal: 0.
	encoder patch: save.
	encoder genHigh: 5 low: c.  " push constant "
	encoder patch: ssave
|
optimizeIf: encoder block: inBlock | save ssave |
	encoder genHigh: 15 low: 7.  " branch if true test "
	save <- encoder genVal: 0.
	arguments first compileInLine: encoder block: inBlock.
	arguments removeFirst.
	encoder genHigh: 15 low: 6.  " branch "
	ssave <- encoder genVal: 0.
	encoder patch: save.
	arguments first compileInLine: encoder block: inBlock.
	encoder patch: ssave
]
Methods Encoder
name: n
	name <- n asSymbol.
	byteCodes <- ByteArray new: 20.
	index <- 0.
	literals <- Array new: 0.
	stackSize <- 0.
	maxStack <- 1.
|
lineNum: l
	" Don't care, except in DebugEncoder subclass "
	^self
|
pushArgs: n
	stackSize <- stackSize + n.
	maxStack <- stackSize max: maxStack
|
popArgs: n
	stackSize <- stackSize - n.
|
genLiteral: aValue | idx |
	idx <- literals indexOf: [:v| v == aValue].
	idx notNil ifTrue: [ ^ idx - 1 ].
	literals <- literals with: aValue.
	^ literals size - 1
|
genHigh: high low: low
	(low >= 16)
		ifTrue: [ self genHigh: 0 low: high. self genCode: low ]
		ifFalse: [ self genCode: high * 16 + low ]
|
genCode: byte
	index <- index + 1.
	(index >= byteCodes size)
		ifTrue: [ self expandByteCodes].
	byteCodes at: index put: byte.
	^ index
|
genVal: byte
	self genCode: (byte rem: 256).
	self genCode: (byte quo: 256).
	^ index-1
|
expandByteCodes	| newarray size |
	size <- byteCodes size.
	newarray <- ByteArray new: size + 8.
	(1 to: size) do: [:i | newarray at: i put: (byteCodes at: i)].
	byteCodes <- newarray
|
patch: loc
		" patch a goto from a block "
	byteCodes at: loc put: ((index+1) rem: 256).
	byteCodes at: (loc + 1) put: ((index+1) quo: 256)
|
currentLocation
	^ index
|
backUp
	" back up one instruction "
	index <- index - 1
|
method: maxTemps class: c text: text
	^ (Method new) message: name;byteCodes: (byteCodes copyFrom:1 to:index);literals: literals;
		stackSize: maxStack; temporarySize: maxTemps;class: c
		";text: text"
]
* external function
Methods Array 'all'
includes: aValue
	self do: [ :element | element = aValue ifTrue: [ ^ true ]].
	^ false
]
Methods Method 'all'
	byteCodes:bc
		bytecodes <- bc
|
	literals:lt
		literals <- lt
|
	stackSize:ss
		stackSize <- ss
|
	temporarySize:ts
		temporarySize <- ts
|
	temporarySize
		^temporarySize
|
	class:cls
		class <- cls
]
Methods Char 'all'
	value 
		^ value
]
Methods Object 'all'
	error: str
		smalltalk error:str
|
	isChar
		^ false
]
Methods String 'all'
	doIt	| meth |
		meth <- UndefinedObject parseMethod: 'doItCommand ' + self.
		^ meth notNil
			ifTrue: [ 
				"meth display."
				meth executeWith:#(0)
			]
|
	asNumber | val |
		" parse a base-10 ASCII number, return nil on failure "
		val <- 0.
		self do: [:c|
			c isDigit ifFalse: [^nil].
			val <- (val * 10) + (c value - 48)
		].
		^val
]
Methods Class 'all'
	instanceVariables
		^ variables
|
	parseMethod: text
		"text print."
		^ (Parser new
			text: text instanceVars: self instanceVariables) parse: self
|
	editMethod:mName |m code str|
		m  <- methods at:mName ifAbsent:[nil].
		code <- ''.
		m isNil ifFalse:[code <- m text].
		str <- (<122 code (code size)>).
		m <- self parseMethod: str.
		m isNil ifTrue:[^ false].
		m text:str.
		methods at:m name put:m
]

