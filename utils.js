function pushValueOnStack(value) {
    const container = document.getElementById('container-vm-stack')
        const divElement = document.createElement('div');
        divElement.innerText = value
        container.prepend(divElement);

        const constantsContainer = document.getElementById('container-constants-table');
        const constantsValueContainer = constantsContainer.getElementsByClassName('constants-value')[0];
        for(child of constantsValueContainer.children) {
            if (child.innerText == value) {
                child.style.border = '3px solid green';
            }
        }
}

function addConstant(constant) {
    const container = document.getElementById('container-constants-table');
    const indexContainer = container.getElementsByClassName('constants-index')[0];
    const valueContainer = container.getElementsByClassName('constants-value')[0];
    const indexEl = document.createElement('div');
    indexEl.innerText = indexContainer.childElementCount;
    indexContainer.appendChild(indexEl);
    const valueEl = document.createElement('div');
    valueEl.innerText = constant;
    valueContainer.appendChild(valueEl);
}

function addEntryToGlobalsTable(key, value) {
    const container = document.getElementById('container-globals-table');
    const keyContainer = container.getElementsByClassName('globals-key')[0];
    const valueContainer = container.getElementsByClassName('globals-value')[0];
    
    //  check if the key container contains a div with the same text already
    const keyContainerChildren = keyContainer.childNodes;
    for (const k of keyContainerChildren) {
        if (k.innerText === key) {
            return
        }
    }    

    const keyElement = document.createElement('div')
    keyElement.innerText = key;
    keyContainer.appendChild(keyElement);

    const valueElement = document.createElement('div');
    valueElement.innerText = value;
    valueContainer.appendChild(valueElement);
}

function getUnderlyingValueOfObject(objPointer) {
    const objType = Module.getValue(objPointer, "i32");
    switch (objType) {
        case 0:
            //  Handling the ObjClosure struct
            objPointer = objPointer + 12;
            const functionPointer = Module.getValue(objPointer, "i32");
            const functionStruct = Module.getValue(functionPointer, "i32");
            const arityPointer = functionStruct + 12;
            const upvaluePointer = arityPointer + 4;
            const chunkPointer = upvaluePointer + 4;
            const stringPointerPointer = chunkPointer + 28;
            const sp = Module.getValue(stringPointerPointer, "i32");
            const s = UTF8ToString(sp);
            if (s === null || s === undefined || s === "") {
                return "<script>";
            } else {
                return s;
            }
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            //  Handling the ObjString struct
            objPointer = objPointer + 12;
            objPointer = objPointer + 4;
            const stringPointer = Module.getValue(objPointer, "i32");
            const string = UTF8ToString(stringPointer);
            return string;
        case 4:
            break;
    }
}

function getUnderlyingValueOfStruct(valuePointer) {
    const valueType = Module.getValue(valuePointer, "i32");
    const valueAsStruct = valuePointer + 8;
    switch(valueType) {
        case 0:
            const bool = Module.getValue(valueAsStruct, "i8");
            return bool;
        case 1:
            return null;
        case 2:
            const double = Module.getValue(valueAsStruct, "double");
            return double;
        case 3:
            let objPointer = Module.getValue(valueAsStruct, "i32");
            const objType = Module.getValue(objPointer, "i32");
            switch (objType) {
                case 0:
                    //  Handling the ObjClosure struct
                    objPointer = objPointer + 12;
                    const functionPointer = Module.getValue(objPointer, "i32");
                    const functionStruct = Module.getValue(functionPointer, "i32");
                    const arityPointer = functionStruct + 12;
                    const upvaluePointer = arityPointer + 4;
                    const chunkPointer = upvaluePointer + 4;
                    const stringPointerPointer = chunkPointer + 28;
                    const sp = Module.getValue(stringPointerPointer, "i32");
                    const s = UTF8ToString(sp);
                    if (s === null || s === undefined || s === "") {
                        return "<script>";
                    } else {
                        return s;
                    }
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    //  Handling the ObjString struct
                    objPointer = objPointer + 12;
                    objPointer = objPointer + 4;
                    const stringPointer = Module.getValue(objPointer, "i32");
                    const string = UTF8ToString(stringPointer);
                    return string;
                case 4:
                    break;
            }
            break;
    }
}

function passStructAddress(value) {
    const valueType = Module.getValue(value, "i32");
    const valueAsStruct = value + 8;
    switch(valueType) {
        case 0:
            console.log("Value boolean is: ", Module.getValue(valueAsStruct, "i8"));
            break;
        case 1:
            break;
        case 2:
            console.log("Value number is: ", Module.getValue(valueAsStruct, "double"));
            break;
        case 3:
            let objPointer = Module.getValue(valueAsStruct, "i32");
            const objType = Module.getValue(objPointer, "i32");
            console.log("Object is: ", objType);
            switch (objType) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    //  Handling the ObjString struct
                    objPointer = objPointer + 12;
                    console.log("The string length is: ", Module.getValue(objPointer, "i32"));
                    objPointer = objPointer + 4;
                    const stringPointer = Module.getValue(objPointer, "i32");
                    const string = UTF8ToString(stringPointer);
                    console.log(string);
                    break;
                case 4:
                    break;
            }
            break;
    }
    functionCall(value);
}