Class JSONParser Object src position
Class JSONTest Object
Methods JSONParser 'all'
src
	^ src
|
new
	position <- 0.
	src <- nil
|
src:aString
	src <- aString
|
position
	^ position
|
position:anIndex
	position <- anIndex
|
currentToken
    position > src size ifTrue:[^nil].
	^ src at:self position
|
nextToken
	(src = nil) ifTrue:[^ nil].
	self position:(position + 1).
	(src size < position) ifTrue:[^ nil].
	^ src at:self position
|
nextWord |tk start end|
	tk <- self currentToken.
	tk isSpaceOrNewLine ifTrue:[
		tk <- self nextTokenIgnoreSpace].
	tk isNil ifTrue:[^nil].
	start <- position.
	[tk <- self nextToken.
	self ifTokenVerify:[
	(tk ~= $,)&(tk ~= $]) & (tk ~= $}) 
		& (tk isSpaceOrNewLine not)]] whileTrue.
	end <- position-1.
	self position:end.
	^ src copyFrom:start to:end
|
nextTokenIgnoreSpace |tk|
	[tk <- self nextToken. 
	self ifTokenVerify:[
	tk isSpaceOrNewLine]] whileTrue.
	^tk
|
parse:aString |obj|
	self src:(aString).
	self position:0.
	obj <- self parse.
	self end.
	^ obj
|
expect:token but:abug
	^smalltalk error:'Parse error at: ', (position printString),
						' expecting ',token,' but receive:',
						(abug printString).
|
expect:token
	^ self expect:token but:self currentToken.
|
parse |token|
	token <- self nextTokenIgnoreSpace.
	token = $" ifTrue:[ ^self parseString].
	token = ${ ifTrue:[^self parseObject].
	token = $[ ifTrue:[^self parseArray].
	((token = $t) | (token = $f) | (token = $n)) ifTrue:[
		^self parseConstant].
	^self parseNumber
|
end
	self nextToken isNil ifFalse:[^self expect:'EOF']
|
parseString |start end tk|
	self currentToken = $" ifFalse:[^self expect:$"].
	start <- position + 1.
	[tk <- self nextToken.
	tk = $\ ifTrue:[ self nextToken]. 
	tk ~= $"] whileTrue.
	tk isNil ifTrue:[^self expect:$"].
	end <- position - 1.
	^ src copyFrom:start to:end
|
parseObject |dict tk key value|
	self currentToken = ${ ifFalse:[^self expect:${].
	dict <- Dictionary new.
	[	tk <- self nextTokenIgnoreSpace.
		key <- self parseString.
		tk <- self nextTokenIgnoreSpace.
		tk = $: ifFalse:[^self expect:$:].
		value <- self parse.
		dict at:key put:value.
		tk <- self nextTokenIgnoreSpace.
		tk = $,] whileTrue.
	tk = $} ifFalse:[^ self expect:$}].
	^ dict
|
parseArray |tk list val|
	self currentToken = $[ ifFalse:[^self expect:$[].
	list <- List new.
	[
		value <- self parse.
		list add:value.
		tk <- self nextTokenIgnoreSpace.
		tk = $,] whileTrue.
	tk = $] ifFalse:[^ self expect:$]].
	^list
|
parseConstant |text|
    (src size >= (position+3)) ifFalse:[^self expect:'in range' but:'out of range'].
    text <- self nextWord.
    text = 'true' ifTrue:[ ^true].
    text = 'false' ifTrue:[^false].
	text = 'null' ifTrue:[^ nil].
    ^ self expect:'constant' but:text.
|
ifTokenVerify:ablock
    self currentToken isNil ifTrue:[^false].
    ^ ablock value.
|

parseNumber |tk text sign start float|
    sign <- false.
    float <- false.
    tk <- self currentToken.
    tk = $- ifTrue:[sign<-true. tk <- self nextToken].
    tk isDigit ifFalse:[^self expect:'digit'].
    start <- position.
    [tk <- self nextToken. self ifTokenVerify:[tk isDigit]] whileTrue.
    tk = $. ifTrue:[
        float <- true.
        [tk <- self nextToken. 
            self ifTokenVerify:[tk isDigit]] whileTrue.
        ((tk = $e) | (tk = $E)) ifTrue:[
            tk <- self nextToken.
            tk isNil ifFalse:[
                ((tk isDigit)|(tk = $+)|(tk = $-)) ifFalse:[
                    ^self expect:'+/-/digit'].
                [tk <- self nextToken. 
                    self ifTokenVerify:[tk isDigit]] whileTrue
            ]
        ]
    ].
    text <- src copyFrom:start to: position-1.
    self position: position - 1.
    float ifTrue:[
        text <- text asFloat
    ] ifFalse:[
        text <- text asInteger
    ].
    sign ifTrue:[^ text negated].
    ^text
]
Methods JSONTest 'all'
data 
    ^'{
    "glossary": {
        "title": "example glossary",
		"GlossDiv": {
            "title": "S",
			"GlossList": {
                "GlossEntry": {
                    "ID": 1078,
					"SortAs": "SGML",
					"GlossTerm": "Standard Generalized Markup Language",
					"Acronym": "SGML",
					"Abbrev": "ISO 8879:1986",
					"GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
						"GlossSeeAlso": ["GML", "XML"]
                    },
					    "GlossSee": 0.45e-10
                    }
                }
            }
        }
    }'



|
data2 
    ^'{"menu": {
    "header": "SVG Viewer",
    "items": [
        {"id": "Open"},
        {"id": "OpenNew", "label": "Open New"},
        null,
        {"id": "ZoomIn", "label": "Zoom In"},
        {"id": "ZoomOut", "label": "Zoom Out"},
        {"id": "OriginalView", "label": "Original View"},
        null,
        {"id": "Quality"},
        {"id": "Pause"},
        {"id": "Mute"},
        null,
        {"id": "Find", "label": "Find..."},
        {"id": "FindAgain", "label": "Find Again"},
        {"id": "Copy"},
        {"id": "About", "label": "About Adobe CVG Viewer..."}
    ]}}'




|
data1 
    ^'{"widget": {
    "debug": "on",
    "window": {
        "title": "Sample Konfabulator Widget",
        "name": "main_window",
        "width": 500,
        "height": 500
    },
    "image": { 
        "src": "Images/Sun.png",
        "name": "sun1",
        "hOffset": 250,
        "vOffset": 250,
        "alignment": "center"
    },
    "text": {
        "data": "Click Here",
        "size": 36,
        "style": "bold",
        "name": "text1",
        "hOffset": 250,
        "vOffset": 100,
        "alignment": "center",
        "onMouseUp": "sun1.opacity = (sun1.opacity / 100) * 90;"
    }
}}'
]
