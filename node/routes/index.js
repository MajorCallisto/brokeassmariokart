
/*
 * GET home page.
 */

exports.index = function(req, res){
	res.render('index', { title: 'Express', ip:"http://192.168.1.7" });
};
