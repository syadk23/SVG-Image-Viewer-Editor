'use strict'

// C library API
const ffi = require('ffi-napi');

let lib = ffi.Library('./libsvgparse', {
  'parseSVGtoJSON': ['string', ['string']],
  'titleDescJSON': ['string', ['string']],
  'getRectJSON': ['string', ['string']],
  'getCircJSON': ['string', ['string']],
  'getPathJSON': ['string', ['string']],
  'getGroupJSON': ['string', ['string']],
  'addCircle': ['bool', ['string', 'string']],
  'addRectangle': ['bool', ['string', 'string']],
  'editTitle': ['bool', ['string', 'string']],
  'editDesc': ['bool', ['string', 'string']],
  'showAttrCirc': ['string', ['string', 'int']],
  'showAttrRect': ['string', ['string', 'int']],
  'showAttrPath': ['string', ['string', 'int']],
  'showAttrGroup': ['string', ['string', 'int']],
  'scaleRects': ['bool', ['string', 'float']],
  'scaleCircles': ['bool', ['string', 'float']],
  'newSVGimage': ['bool', ['string']],
  'editAttribute': ['bool', ['string', 'string', 'int', 'string', 'string']],
  'editRect': ['bool', ['string', 'int', 'string', 'string']],
  'editRectUnits': ['bool', ['string', 'int', 'string', 'string']],
  'editCirc': ['bool', ['string', 'int', 'string', 'string']],
  'editCircUnits': ['bool', ['string', 'int', 'string', 'string']],
  'editPath': ['bool', ['string', 'int', 'string']],
});

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');
const testFolder = './uploads/';

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;

  if (uploadFile === undefined)
  {
    return res.status(200).send('No File Selected');
  }

  if (fs.existsSync('uploads/' + uploadFile.name))
  {
    return res.status(200).send('You cannot overwrite existing files');
  }
  else
  {
    // Use the mv() method to place the file somewhere on your server
    uploadFile.mv('uploads/' + uploadFile.name, function(err) {
      if(err) {
        return res.status(500).send(err);
      }
      res.redirect('/');
    });
  }
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

app.get('/getFiles', function(req, res)
{
  let filenamesArray = [];
  let filesizesArray = [];
  let objArray = [];

  fs.readdir(testFolder, (err, files) => {
    files.forEach(file => {

      let SVGJSON = lib.parseSVGtoJSON('./uploads/' + file);
      let retObj = JSON.parse(SVGJSON);

      let stats = fs.statSync("uploads/" + file);
      let fileSize = stats["size"];
      filenamesArray.push(file);
      filesizesArray.push(fileSize);
      objArray.push(retObj);
    });
    res.send({
      filenames: filenamesArray,
      filesizes: filesizesArray,
      JSONObj: objArray,
    });
  });
});

app.get('/showSVGPanel', function(req, res)
{
  getData(req, res);
});

app.get('/editTitle', function(req, res)
{
  let bool = lib.editTitle('./uploads/' + req.query.selectedID, req.query.formValue);
  if (bool == true)
  {
    getData(req, res);
  }
  else
  {
    res.send({
      fail: bool,
    });
  }
});

app.get('/editDesc', function(req, res)
{
  let bool = lib.editDesc('./uploads/' + req.query.selectedID, req.query.formValue);
  if (bool == true)
  {
    getData(req, res);
  }
  else
  {
    res.send({
      fail: bool,
    });
  }
});


app.get('/addCircle', function(req, res)
{ 
  let JSONstring = '{"cx":' + req.query.cx + ',"cy":' + req.query.cy + ',"r":' + req.query.r + ',"units":"' + req.query.units + '"}';
  let bool = lib.addCircle('./uploads/' + req.query.selectedID, JSONstring);
  if (bool == true)
  {
    getData(req, res);
  }
  else
  {
    res.send({
      fail: bool,
    });
  }
});

app.get('/addRectangle', function(req, res)
{  
  let JSONstring = '{"x":' + req.query.x + ',"y":' + req.query.y + ',"w":' + req.query.w + ',"h":' + req.query.h + ',"units":"' + req.query.units + '"}';
  let bool = lib.addRectangle('./uploads/' + req.query.selectedID, JSONstring);
  if (bool == true)
  {
    getData(req, res);
  }
  else
  {
    res.send({
      fail: bool,
    });
  }
});

app.get('/showAttrCirc', function(req, res)
{
  let string = lib.showAttrCirc('./uploads/' + req.query.selectedID, req.query.index);
  res.send({
    retStr: string,
  });
});

app.get('/showAttrRect', function(req, res)
{
  let string = lib.showAttrRect('./uploads/' + req.query.selectedID, req.query.index);
  res.send({
    retStr: string,
  });
});

app.get('/showAttrPath', function(req, res)
{
  let string = lib.showAttrPath('./uploads/' + req.query.selectedID, req.query.index);
  res.send({
    retStr: string,
  });
});

app.get('/showAttrGroup', function(req, res)
{
  let string = lib.showAttrGroup('./uploads/' + req.query.selectedID, req.query.index);
  res.send({
    retStr: string,
  });
});

app.get('/scaleShapes', function(req, res)
{
  let bool1 = lib.scaleRects('./uploads/' + req.query.selectedID, req.query.scaleRect);
  let bool2 = lib.scaleCircles('./uploads/' + req.query.selectedID, req.query.scaleCirc);
  if (bool1 == true && bool2 == true)
    getData(req, res);
});

app.get('/createSVG', function(req, res)
{
  if (!fs.existsSync('uploads/' + req.query.formValue))
  {
    let bool = lib.newSVGimage(req.query.formValue);
    if (bool == true)
    {
      let filenamesArray = [];
      let filesizesArray = [];
      let objArray = [];

      fs.readdir(testFolder, (err, files) => {
        files.forEach(file => {

          let SVGJSON = lib.parseSVGtoJSON('./uploads/' + file);
          let retObj = JSON.parse(SVGJSON);

          let stats = fs.statSync("uploads/" + file);
          let fileSize = stats["size"];
          filenamesArray.push(file);
          filesizesArray.push(fileSize);
          objArray.push(retObj);
        });
        res.send({
          filenames: filenamesArray,
          filesizes: filesizesArray,
          JSONObj: objArray,
        });
      });
    }
  }
  else
  {
    res.send({
      fail: false,
    });
  }
});

app.get('/editAttr', function(req, res)
{
  let bool = lib.editAttribute('./uploads/' + req.query.selectedID, req.query.shape, req.query.index, req.query.attrName, req.query.attrValue);
  if (bool == true)
    getData(req, res);
  else
  {
    res.send({
      fail: bool,
    });
  }
});

app.get('/editRect', function(req, res)
{
  let bool = lib.editRect('./uploads/' + req.query.selectedID, req.query.index, req.query.attrName, req.query.attrValue);
  if (bool == true)
    getData(req, res);
  else
  {
    res.send({
      fail: bool,
    });
  }
});

app.get('/editRectUnits', function(req, res)
{
  let bool = lib.editRectUnits('./uploads/' + req.query.selectedID, req.query.index, req.query.attrName, req.query.attrValue);
  if (bool == true)
    getData(req, res);
  else
  {
    res.send({
      fail: bool,
    });
  }
});

app.get('/editCirc', function(req, res)
{
  let bool = lib.editCirc('./uploads/' + req.query.selectedID, req.query.index, req.query.attrName, req.query.attrValue);
  if (bool == true)
    getData(req, res);
  else
  {
    res.send({
      fail: bool,
    });
  }
});

app.get('/editCircUnits', function(req, res)
{
  let bool = lib.editCircUnits('./uploads/' + req.query.selectedID, req.query.index, req.query.attrName, req.query.attrValue);
  if (bool == true)
    getData(req, res);
  else
  {
    res.send({
      fail: bool,
    });
  }
});

app.get('/editPath', function(req, res)
{
  let bool = lib.editPath('./uploads/' + req.query.selectedID, req.query.index, req.query.attrValue);
  if (bool == true)
    getData(req, res);
  else
  {
    res.send({
      fail: bool,
    });
  }
});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);



function getData(req, res)
{
  let Rects = [];
  let Circles = [];
  let Paths = [];
  let Groups = [];

  let titleandDescJSON = lib.titleDescJSON('./uploads/' + req.query.selectedID);
  titleandDescJSON = titleandDescJSON.replace(/\n/g, "\\n").replace(/\r/g, "\\r").replace(/\t/g, "\\t");
  let SVGTitleandDesc = JSON.parse(titleandDescJSON);

  let rectListJSON = lib.getRectJSON('./uploads/' + req.query.selectedID);
  let rectList = JSON.parse(rectListJSON);

  let circListJSON = lib.getCircJSON('./uploads/' + req.query.selectedID);
  let circList = JSON.parse(circListJSON);

  let pathListJSON = lib.getPathJSON('./uploads/' + req.query.selectedID);
  let pathList = JSON.parse(pathListJSON);

  let groupListJSON = lib.getGroupJSON('./uploads/' + req.query.selectedID);
  let groupList = JSON.parse(groupListJSON);

  for (let i = 0; i < rectList.length; i++)
  {
    Rects.push(rectList[i]);
  }

  for (let i = 0; i < circList.length; i++)
  {
    Circles.push(circList[i]);
  }

  for (let i = 0; i < pathList.length; i++)
  {
    Paths.push(pathList[i]);
  }

  for (let i = 0; i < groupList.length; i++)
  {
    Groups.push(groupList[i]);
  }
  res.send({
    filenames: req.query.selectedID,
    title: SVGTitleandDesc.title,
    description: SVGTitleandDesc.description,
    rectangles: Rects,
    circles: Circles,
    paths: Paths,
    groups: Groups,
  });
}
