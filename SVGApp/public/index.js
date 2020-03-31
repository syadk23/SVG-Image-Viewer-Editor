// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getFiles',   //The server endpoint we are connecting to
        data: {
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            //We write the object to the console to show that the request was successful
            addFileLog(data.filenames, data.filesizes, data.JSONObj);
            addDropDown(data.filenames, data.JSONObj);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log('error is ' + error); 
        }
    });

    $('#images').change(function(e)
    {
        let imgSelected = $("#images option:selected").text();
        if (imgSelected == '')
        {
            $('#SVG_view_panel').empty();
            return;
        }

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/showSVGPanel',   //The server endpoint we are connecting to
            data: {
                selectedID: imgSelected,
            },
            success: function (data) {
                addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log('error is ' + error); 
            }
        });
    });

    $('#addRectangle').click(function(e)
    {
        let imgSelected = $("#images option:selected").text();
        if (imgSelected == '')
        {
            alert('Please select an image before adding a shape.');
            return;
        }

        let xValue = parseFloat(document.getElementById('x').value);
        let yValue = parseFloat(document.getElementById('y').value);
        let wValue = parseFloat(document.getElementById('w').value);
        let hValue = parseFloat(document.getElementById('h').value);
        let units = document.getElementById('rectUnits').value;
            
        $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/addRectangle',
                data: {
                    selectedID: imgSelected,
                    x: xValue,
                    y: yValue,
                    w: wValue,
                    h: hValue,
                    units: units,
                },
                success: function (data) {
                    if (data.fail == false)
                    {
                        alert('You cannot enter invalid attributes (Eg. negative width or height');
                        return;
                    }
                    addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                    $('#rectModal').modal('hide');
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    console.log('error is ' + error); 
                }
        });
    });

    $('#addCircle').click(function(e)
    {
        let imgSelected = $("#images option:selected").text();
        if (imgSelected == '')
        {
            alert('Please select an image before adding a shape.');
            return;
        }

        let cxValue = parseFloat(document.getElementById('cx').value);
        let cyValue = parseFloat(document.getElementById('cy').value);
        let rValue = parseFloat(document.getElementById('r').value);
        let units = document.getElementById('circUnits').value;
            
        $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/addCircle',
                data: {
                    selectedID: imgSelected,
                    cx: cxValue,
                    cy: cyValue,
                    r: rValue,
                    units: units,
                },
                success: function (data) {
                    if (data.fail == false)
                    {
                        alert('You cannot enter invalid attributes (Eg. negative radius');
                        return;
                    }
                    addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                    $('#circModal').modal('hide');
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    console.log('error is ' + error); 
                }
        });
    });

    $('#editTitle').click(function(e)
    {
        let imgSelected = $("#images option:selected").text();
        if (imgSelected == '')
        {
            alert('Please select an image before editing the title/description.');
            return;
        }

        $('#titleForm').empty();
        $('#titleForm').append('<br><label>Enter a Title</label> <input type = "text" id = "titleText" name = "title" value = ""><input type = "submit" value = "Submit">');

    });

    $('#titleForm').submit(function(e)
    {
        let formValue = document.getElementById('titleText').value;
        let imgSelected = $("#images option:selected").text();

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/editTitle',
            data: {
                selectedID: imgSelected,
                formValue: formValue,
            },
            success: function (data) {
                if (data.fail == false)
                {
                    alert('Error in changing title - fails to validate SVG format');
                    return;
                }
                addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                location.reload();
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log('error is ' + error); 
            }
        });
    });

    $('#editDesc').click(function(e)
    {
        let imgSelected = $("#images option:selected").text();
        if (imgSelected == '')
        {
            alert('Please select an image before editing the title/description.');
            return;
        }

        $('#descForm').empty();
        $('#descForm').append('<br><label>Enter a Description</label> <input type = "text" id = "descText" name = "description" value = ""><input type = "submit" value = "Submit">');

    });

    $('#descForm').submit(function(e)
    {
        let formValue = document.getElementById('descText').value;
        let imgSelected = $("#images option:selected").text();

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/editDesc',
            data: {
                selectedID: imgSelected,
                formValue: formValue,
            },
            success: function (data) {
                if (data.fail == false)
                {
                    alert('Error in changing description - fails to validate SVG format');
                    return;
                }
                addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                location.reload();
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log('error is ' + error); 
            }
        });
    });

    $('#scaleAccept').click(function(e)
    {
        let imgSelected = $("#images option:selected").text();
        let rectScaleSliderVal = document.getElementById('rectSliderText').value;
        let circScaleSliderVal = document.getElementById('circSliderText').value;

        if (imgSelected == '')
        {
            alert('Please select an image before scaling shapes.');
            return;
        }

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/scaleShapes',
            data: {
                selectedID: imgSelected,
                scaleRect: rectScaleSliderVal,
                scaleCirc: circScaleSliderVal,
            },
            success: function (data) {
                addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                location.reload();
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log('error is ' + error); 
            }
        });
    });

    $('#createSVG').click(function(e)
    {
        $('#createSVGForm').empty();
        $('#createSVGForm').append('<br><label>Enter a Filename for the SVG image</label> <input type = "text" accept = ".svg" id = "createSVGText" name = "createSVGText" value = ""><input type = "submit" value = "Submit">');
    });

    $('#createSVGForm').submit(function(e)
    {
        let formValue = document.getElementById('createSVGText').value;
        if (formValue.includes(".svg") == false)
        {
            formValue = formValue.concat(".svg");
        }

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/createSVG',
            data: {
                formValue: formValue,
            },
            success: function (data) {
                if (data.fail == false)
                {
                    alert('File already exists with that name!');
                    return;
                }
                addFileLog(data.filenames, data.filesizes, data.JSONObj);
                addDropDown(data.filenames, data.JSONObj);
                location.reload();
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log('error is ' + error); 
            }
        });
    });

    $('#submitFile').click(function(req, res)
    {
        if(document.getElementById("myFile").value != "")
        {
            alert('File added successfully');
            location.reload();
        }
    });

    $('#attrAccept').click(function(req, res)
    {
        let i = 1;
        let imgSelected = $("#images option:selected").text();
        let component = document.getElementById('componentSelected').value;
        let shape = 'none';
        let index;
        let attrName = document.getElementById('attrName').value;
        let attrValue = document.getElementById('attrValue').value;
        while(document.getElementById('rectangle' + i) != null)
        {
            if (component == document.getElementById('rectangle' + i).id)
            {
                shape = 'rectangle';
                index = i - 1;
            }
            i++;
        }
        i = 1;
        while(document.getElementById('circle' + i) != null)
        {
            if (component == document.getElementById('circle' + i).id)
            {
                shape = 'circle';
                index = i - 1;
            }
            i++;
        }
        i = 1;
        while(document.getElementById('path' + i) != null)
        {
            if (component == document.getElementById('path' + i).id)
            {
                shape = 'path';
                index = i - 1;
            }
            i++;
        }
        i = 1;
        while(document.getElementById('group' + i) != null)
        {
            if (component == document.getElementById('group' + i).id)
            {
                shape = 'group';
                index = i - 1;
            }
            i++;
        }
        i = 1;
        if (shape == 'none')
        {
            alert('Please enter a proper component');
            return;
        }
        
        if (shape == 'rectangle' && attrName == 'units')
        {
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/editRectUnits',
                data: {
                    shape: shape,
                    selectedID: imgSelected,
                    index: index,
                    attrName: attrName,
                    attrValue: attrValue,
                },
                success: function (data) {
                    if (data.fail == false)
                    {
                        alert('Invalid Attribute Value');
                        return;
                    }
                    addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                    $('#attrModal').modal('hide');
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    console.log('error is ' + error); 
                }
            });
        }
        else if (shape == 'circle' && attrName == 'units')
        {
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/editCircUnits',
                data: {
                    shape: shape,
                    selectedID: imgSelected,
                    index: index,
                    attrName: attrName,
                    attrValue: attrValue,
                },
                success: function (data) {
                    if (data.fail == false)
                    {
                        alert('Invalid Attribute Value');
                        return;
                    }
                    addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                    $('#attrModal').modal('hide');
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    console.log('error is ' + error); 
                }
            });
        }
        else if (shape == 'circle' && (attrName == 'cx' || attrName == 'cy' || attrName == 'r'))
        {
            if (isNaN(attrValue))
            {
                alert('Attribute Value must be a number to continue!');
            }
            else
            {
                $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/editCirc',
                    data: {
                        shape: shape,
                        selectedID: imgSelected,
                        index: index,
                        attrName: attrName,
                        attrValue: attrValue,
                    },
                    success: function (data) {
                        if (data.fail == false)
                        {
                            alert('Invalid Attribute Value');
                            return;
                        }
                        addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                        $('#attrModal').modal('hide');
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        console.log('error is ' + error); 
                    }
                });
            }
        }
        else if (shape == 'rectangle' && (attrName == 'x' || attrName == 'y' || attrName == 'w' || attrName || 'h'))
        {
            if (isNaN(attrValue))
            {
                alert('Attribute Value must be a number to continue!');
            }
            else
            {
                $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/editRect',
                    data: {
                        shape: shape,
                        selectedID: imgSelected,
                        index: index,
                        attrName: attrName,
                        attrValue: attrValue,
                    },
                    success: function (data) {
                        if (data.fail == false)
                        {
                            alert('Invalid Attribute Value');
                            return;
                        }
                        addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                        $('#attrModal').modal('hide');
                    },
                    fail: function(error) {
                        // Non-200 return, do something with error
                        console.log('error is ' + error); 
                    }
                });
            }
        }
        else if (shape == 'path' && attrName == 'd')
        {
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/editPath',
                data: {
                    shape: shape,
                    selectedID: imgSelected,
                    index: index,
                    attrName: attrName,
                    attrValue: attrValue,
                },
                success: function (data) {
                    if (data.fail == false)
                    {
                        alert('Invalid Path Data');
                        return;
                    }
                    addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                    $('#attrModal').modal('hide');
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    console.log('error is ' + error); 
                }
            });
        }
        else
        {
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/editAttr',
                data: {
                    shape: shape,
                    selectedID: imgSelected,
                    index: index,
                    attrName: attrName,
                    attrValue: attrValue,
                },
                success: function (data) {
                    if (data.fail == false)
                    {
                        alert('Invalid Attribute Name and/or Value');
                        return;
                    }
                    addSVGLog(data.filenames, data.title, data.description, data.rectangles, data.circles, data.paths, data.groups);
                    $('#attrModal').modal('hide');
                },
                fail: function(error) {
                    // Non-200 return, do something with error
                    console.log('error is ' + error); 
                }
            });
        }
    });
});

function addFileLog(filenames, filesizes, JSONObj)
{
    $('#file_log_panel').empty();
    $('#file_log_panel').append('<tbody><tr bgcolor = "#000000"> <th width = "200%"><b>Image (Click to Download)</b></th><th><b>File Name (Click to Download)</b></th><th><b>File Size (KB)</b></th><th><b>Number of Rectangles</b></th><th><b>Number of Circles</b></th><th><b>Number of Paths</b></th><th><b>Number of Groups</b></th></tr></tbody>');
    
    if (filenames.length == 0)
    {
        $('#file_log_panel').empty();
        $('#file_log_panel').append('<b><p>NO FILES ON THE SERVER, PLEASE UPLOAD OR CREATE ONE</b></p>');
    }

    for (i = 0; i < filenames.length; i++)
    {
        if (JSONObj[i].numRect != null)
            $('#file_log_panel').append('<tr><td bgcolor = "#ffffff"><a download = "' + filenames[i] + '" href = "' + filenames[i] + '" title = "' + filenames[i] + '"> <img width = "200px" alt = "' + filenames[i] + '" src = "' + filenames[i] + '"></a></td> <td> <a href = "' + filenames[i] + '">' + filenames[i] + '</a></td>' + '<td>' + Math.round((filesizes[i])/1000) + '</td> <td>'  + JSONObj[i].numRect + '</td> <td>' + JSONObj[i].numCirc + '</td> <td>' + JSONObj[i].numPaths + '</td> <td>' + JSONObj[i].numGroups);
    }
}

function addDropDown(filenames, JSONObj)
{
    $('#images').empty();
    $('#images').append('<option></option>');
    for (i = 0; i < filenames.length; i++)
    {
        if (JSONObj[i].numRect != null)
            $('#images').append('<option value' + filenames[i] + '>' + filenames[i] + '</option>');
    }
}

function addSVGLog(filenames, title, description, rectangles, circles, paths, groups)
{
    $('#SVG_view_panel').empty();
    $('#SVG_view_panel').append('<tbody> <tr bgcolor = "#ffffff"> <td colspan = "3" width = "850">  <a download = "' + filenames + '"href = "' + filenames + '" title = "' + filenames + '"> <img width = "800px" alt = "' + filenames + '" src = "' + filenames + '"></a></td></tr>');
    $('#SVG_view_panel').append('<tr bgcolor = "#000000"> <th>Title</th> <th colspan = "2">Description</th></tr>');
    $('#SVG_view_panel').append('<tr> <td>' + title + '</td> <td colspan = "2">' + description + '</td></tr>');
    $('#SVG_view_panel').append('<tr bgcolor = "#000000"> <th>Component</th> <th>Summary</th> <th>Other Attributes</th></tr>');

    for (let i = 0; i < rectangles.length; i++)
    {
        if (rectangles[i].numAttr == 0)    
            $('#SVG_view_panel').append('<tr id = "rectangle' + (i+1) + '"> <td> Rectangle ' + (i+1) + '</td> <td>Upper Left Corner: x = ' + rectangles[i].x + rectangles[i].units + ' y = ' + rectangles[i].y + rectangles[i].units + ' w = ' + rectangles[i].w + rectangles[i].units + ' h = ' + rectangles[i].h + rectangles[i].units + '</td> <td>' + rectangles[i].numAttr + '</td></tr>');
        else
            $('#SVG_view_panel').append('<tr id = "rectangle' + (i+1) + '"> <td> Rectangle ' + (i+1) + '</td> <td>Upper Left Corner: x = ' + rectangles[i].x + rectangles[i].units + ' y = ' + rectangles[i].y + rectangles[i].units + ' w = ' + rectangles[i].w + rectangles[i].units + ' h = ' + rectangles[i].h + rectangles[i].units + '</td> <td>' + rectangles[i].numAttr + '<br><button type = "button" class = "rectButton" id = "showAttrButtonRect' + (i+1) + '">Show Attributes</button></td></tr>');
    }

    for (let i = 0; i < circles.length; i++)
    {
        if (circles[i].numAttr == 0)
            $('#SVG_view_panel').append('<tr id = "circle' + (i+1) + '"> <td> Circle ' + (i+1) + '</td> <td>Centre: cx = ' + circles[i].cx + circles[i].units + ' cy = ' + circles[i].cy + circles[i].units + ' r = ' + circles[i].r + circles[i].units + '</td> <td>' + circles[i].numAttr + '</td></tr>');
        else
            $('#SVG_view_panel').append('<tr id = "circle' + (i+1) + '"> <td> Circle ' + (i+1) + '</td> <td>Centre: cx = ' + circles[i].cx + circles[i].units + ' cy = ' + circles[i].cy + circles[i].units + ' r = ' + circles[i].r + circles[i].units + '</td> <td>' + circles[i].numAttr + '<br><button type = "button" class = "circButton" id = "showAttrButtonCirc' + (i+1) + '">Show Attributes</button></td></tr>');
    }

    for (let i = 0; i < paths.length; i++)
    {
        if (paths[i].numAttr == 0)
            $('#SVG_view_panel').append('<tr id = "path' + (i+1) + '"> <td> Path ' + (i+1) + '</td> <td>Path Data = ' + paths[i].d + '</td> <td>' + paths[i].numAttr + '</td></tr>');
        else
            $('#SVG_view_panel').append('<tr id = "path' + (i+1) + '"> <td> Path ' + (i+1) + '</td> <td>Path Data = ' + paths[i].d + '</td> <td>' + paths[i].numAttr + '<br><button type = "button" class = "pathButton" id = "showAttrButtonPath' + (i+1) + '">Show Attributes</button></td></tr>');
    }

    for (let i = 0; i < groups.length; i++)
    {
        if (groups[i].numAttr == 0)
            $('#SVG_view_panel').append('<tr id = "group' + (i+1) + '"> <td> Group ' + (i+1) + '</td> <td>Child Elements = ' + groups[i].children + '</td> <td>' + groups[i].numAttr + '</td></tr>');
        else
            $('#SVG_view_panel').append('<tr id = "group' + (i+1) +'"> <td> Group ' + (i+1) + '</td> <td>Child Elements = ' + groups[i].children + '</td> <td>' + groups[i].numAttr + '<br><button type = "button" class = "groupButton" id = "showAttrButtonGroup' + (i+1) + '">Show Attributes</button></td></tr>');
    }

    let elem = document.getElementsByClassName("rectButton");

    for (let i = 0; i < elem.length; i++)
    {
        (function()
        {
            elem[i].addEventListener("click", function() {getRectAttrBTN(elem[i].id);}, false);
        }());
    }

    let elem2 = document.getElementsByClassName("circButton");

    for (let i = 0; i < elem2.length; i++)
    {
        (function()
        {
            elem2[i].addEventListener("click", function() {getCircAttrBTN(elem2[i].id);}, false);
        }());
    }

    let elem3 = document.getElementsByClassName("pathButton");

    for (let i = 0; i < elem3.length; i++)
    {
        (function()
        {
            elem3[i].addEventListener("click", function() {getPathAttrBTN(elem3[i].id);}, false);
        }());
    }

    let elem4 = document.getElementsByClassName("groupButton");

    for (let i = 0; i < elem4.length; i++)
    {
        (function()
        {
            elem4[i].addEventListener("click", function() {getGroupAttrBTN(elem4[i].id);}, false);
        }());
    }
}

function updateSliderRect(val)
{
    document.getElementById('rectSliderText').value = val;
}

function updateSliderCirc(val)
{
    document.getElementById('circSliderText').value = val;
}

function getRectAttrBTN(id)
{
    let imgSelected = $("#images option:selected").text();
    let thenum = id.replace( /^\D+/g, '');
    thenum = parseInt(thenum);
    
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/showAttrRect',
        data: {
            selectedID: imgSelected,
            index: thenum,
        },
        success: function (data)
        {
            let attrs = '';
            let JSONstring = JSON.parse(data.retStr);
            for (var key in JSONstring)
            {
                attrs = attrs + Object.values(JSONstring[key]);
                attrs = attrs + '\n';
            }
            alert(attrs);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log('error is ' + error); 
        }
    });
}

function getCircAttrBTN(id)
{
    let imgSelected = $("#images option:selected").text();
    let thenum = id.replace( /^\D+/g, '');
    thenum = parseInt(thenum);
    
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/showAttrCirc',
        data: {
            selectedID: imgSelected,
            index: thenum,
        },
        success: function (data)
        {
            let attrs = '';
            let JSONstring = JSON.parse(data.retStr);
            for (var key in JSONstring)
            {
                attrs = attrs + Object.values(JSONstring[key]);
                attrs = attrs + '\n';
            }
            alert(attrs);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log('error is ' + error); 
        }
    });
}

function getPathAttrBTN(id)
{
    let imgSelected = $("#images option:selected").text();
    let thenum = id.replace( /^\D+/g, '');
    thenum = parseInt(thenum);
    
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/showAttrPath',
        data: {
            selectedID: imgSelected,
            index: thenum,
        },
        success: function (data)
        {
            let attrs = '';
            let JSONstring = JSON.parse(data.retStr);
            for (var key in JSONstring)
            {
                attrs = attrs + Object.values(JSONstring[key]);
                attrs = attrs + '\n';
            }
            alert(attrs);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log('error is ' + error); 
        }
    });
}

function getGroupAttrBTN(id)
{
    let imgSelected = $("#images option:selected").text();
    let thenum = id.replace( /^\D+/g, '');
    thenum = parseInt(thenum);
    
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/showAttrGroup',
        data: {
            selectedID: imgSelected,
            index: thenum,
        },
        success: function (data)
        {
            let attrs = '';
            let JSONstring = JSON.parse(data.retStr);
            for (var key in JSONstring)
            {
                attrs = attrs + Object.values(JSONstring[key]);
                attrs = attrs + '\n';
            }
            alert(attrs);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log('error is ' + error); 
        }
    });
}