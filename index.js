const { request, response } = require('express');
const express = require('express');
'use strict';
const fs = require('fs');

const app = express();
const port = process.env.PORT || 3000;
app.listen(port, () => console.log(`listening at ${port}`));
app.use(express.static('public/'));
app.use(express.json());
app.post('/postpath', (request, response) => {
	console.log(request.body.val);
	response.json({
        status: "success"
    });
    response.end();	
	fs.readFile(request.body.val, 'utf8', function(err, data){
		if (err) throw err;
		obj = JSON.parse(data);
		console.log(obj);
	});
})