
/*
 * GET home page.
 */

exports.index = function(req, res){
	res.render('index', { title: 'Express', ip:"http://169.254.247.182" });
};