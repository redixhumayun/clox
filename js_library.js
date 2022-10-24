mergeInto(LibraryManager.library, {
    removeStyles: function() {
        const constantsContainer = document.getElementById('container-constants-table');
        const constantsValueContainer = constantsContainer.getElementsByClassName('constants-value')[0];
        for(child of constantsValueContainer.children) {
            child.style.border = 'none';
        }
    },

    pushedValueOnStack: function(valuePointer) {
        const value = getUnderlyingValueOfStruct(valuePointer);
        pushValueOnStack(value);
    },

    poppedValueFromStack: function() {
        const container = document.getElementById('container-vm-stack');
        let containerElements = container.children;
        const elements = Array.from(containerElements);
        if (elements[0] !== undefined && elements[0] !== null) {
            elements[0].remove();    
        }
    },
    
    iterateOverConstantValues: function(valuePointer, count) {
        while(count > 0) {
            const value = getUnderlyingValueOfStruct(valuePointer);
            valuePointer += 16;
            count--;
            addConstant(value);
        }
    },

    iterateOverTableValues: function(tableEntryPointer, count, array, arrayLength) {
        let updatedPointer = tableEntryPointer;
        while(arrayLength > 0) {
            const arrayValue = Module.getValue(array, "i32");
            updatedPointer = tableEntryPointer + arrayValue * 24;
            let tableEntry = Module.getValue(updatedPointer, "i32");
            const string = getUnderlyingValueOfObject(tableEntry);
            updatedPointer += 8;
            const value = getUnderlyingValueOfStruct(tableEntry);
            addEntryToGlobalsTable(string, value);
            array += 4;
            arrayLength--;
        }
    },

    referenceStruct: function(value) {
        passStructAddress(value);
    },

    vmExecutionFinished: function() {
        const elem = document.getElementById('run-next-iter-button');
        elem.style.display = 'none';
        alert("Done executing!");
    },
});