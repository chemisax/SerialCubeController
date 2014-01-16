animation = new Array();
currentScene = 0;
currentFrame = 0;

var touch = navigator.userAgent.match(/iPad/i) != null;

$(document).ready(function() {
	document.ontouchmove = function(e) {
		e.preventDefault();
	}

	console.log("start");
	loadXML();
});

function loadXML() {
	console.log("loading file");
	$.get("animation.xml", function (data){
		console.log("file loaded");
		title = $(data).find('animation').attr('name');
		animation[0] = title;
		animation[1] = new Array();
		$(data).find('scene').each(function (index) {
			animation[1][index] = new Array();
			animation[1][index][0] = $(this).attr('name');
			animation[1][index][1] = new Array();
			var numFrames = $(this).find('cuadro').size();
			for (i=0;i<numFrames;i++) {
				animation[1][index][1][i] = new Array();
				for (j=0;j<4;j++) {
					currentString = $(this).find('cuadro').eq(i).find('cube').eq(j).text();
					animation[1][index][1][i][j] = new Array();
					for (p=0;p<64;p++) {
						if (currentString.charAt(p) == "1") animation[1][index][1][i][j][p] = 1;
						else animation[1][index][1][i][j][p] = 0;
					}
				}
			}
		});
		setup();
	});
}

function setup () {
		
	$('li#title').text(animation[0]);
	
	str = (touch) ? "touchstart" : "click";
	
	alert(str);
	
	$("#title").on(str, function () {
		animation[0] = window.prompt("New title:", animation[0]);
		$('li#title').text(animation[0]);
	});
	
	$("#scene").on(str, function () {
		animation[1][currentScene][0] = window.prompt("New Scene title:", animation[1][currentScene][0]);
		$('li#scene').text(animation[1][currentScene][0]);
	});
	
	$("#scene-next").on(str, function () {
		totalScenes = animation[1].length-1;
		if (currentScene != totalScenes) {
			saveframe();
			currentScene++;
			currentFrame = 0;
			updateInterface();
		}
	});
	
	$("#scene-prev").on(str, function () {
		if (currentScene != 0) {
			saveframe();
			currentScene--;
			currentFrame = 0;
			updateInterface();
		} 
	});
	
	$("#frame-next").on(str, function () {
		frames = animation[1][currentScene][1].length-1;
		if (currentFrame != frames) {
			saveframe();
			currentFrame++;
			updateInterface();
		}
	});
	
	$("#frame-prev").on(str, function () {
		if (currentFrame != 0) {
			saveframe();
			currentFrame--;
			updateInterface();
		}
	});
	
	$("#new-scene").on(str, function () {
		saveframe();
		newSceneId = animation[1].length;
		animation[1][newSceneId] = new Array();
		animation[1][newSceneId][0] = "NEW SCENE";
		animation[1][newSceneId][1] = new Array ();
		animation[1][newSceneId][1][0] = new Array ();
		for (i=0;i<4;i++){
			animation[1][newSceneId][1][0][i] = new Array();
			for (j=0;j<64;j++) {
				animation[1][newSceneId][1][0][i][j] = 0;
			}
		}
		currentScene = newSceneId;
		currentFrame = 0;
		
		updateInterface();
		console.log("new scene");
		console.log(animation);
	});
	
	$("#new-frame").on(str, function () {
		saveframe();
		fid = animation[1][currentScene][1].length;
		animation[1][currentScene][1][fid] = new Array();
		for (i=0;i<4;i++){
			animation[1][currentScene][1][fid][i] = new Array();
			for (j=0;j<64;j++) {
				animation[1][currentScene][1][fid][i][j] = 0;
			}
		}
		currentFrame = fid;
		updateInterface();
		console.log("new frame");
		console.log(animation);
	});
	
	$("#del-frame").on(str, function () {
		if (animation[1][currentScene][1].length > 1) {
			animation[1][currentScene][1].splice(currentFrame,1);
			console.log("del frame");
			console.log(animation);
			currentFrame--;
			updateInterface();
			
		}
	});
	
	$("#del-scene").on(str, function () {
		if (animation[1].length > 1) {
			animation[1].splice(currentScene,1);
			console.log("del scene");
			console.log(animation);
			currentScene--;
			currentFrame=0;
			updateInterface();
			
		} 
	});
	
	$("#save").on(str, function (){
		saveframe();
		createFile();
	});
	
	updateInterface();
}

function updateInterface () {
	$('li#scene').text(animation[1][currentScene][0]);
	$('#num-scene').text("scene "+(currentScene+1)+"/"+animation[1].length);
	$('#num-frame').text("frame "+(currentFrame+1)+"/"+animation[1][currentScene][1].length);
	
	drawCubes();
}

function drawCubes () {
	$("#a_canvas").html("");
	for (i=0;i<4;i++) {
		$("#a_canvas").append('<div class="cube" id="cube'+i+'"><span>Cube '+(i+1)+'</span></div>');
		for (j=0;j<64;j++) {
			id = "c"+i+"l"+j;
			$("#cube"+i).append('<input type="checkbox" id="'+id+'">');
			value = animation[1][currentScene][1][currentFrame][i][j];
			if (value == 1) {
				$("#"+id).prop('checked', true);
			} else {
				$("#"+id).prop('checked', false);
			}
			
			if ((j+1)%16 == 0 && j!=63) {
				$("#cube"+i).append('<span class="layer-spacer"></span>');
			}	
		}
	}
}

function saveframe () {
	for (i=0; i<4; i++) {
		for (j=0;j<64;j++) {
			id = "#c"+i+"l"+j;
			if ($(id).is(':checked')) {
				animation[1][currentScene][1][currentFrame][i][j] = 1;
			} else {
				animation[1][currentScene][1][currentFrame][i][j] = 0;
			}
		}
	}
}

function createFile (){
	console.log("saving");
	file = '<animation name="'+animation[0]+'">';
	for (i=0;i<animation[1].length;i++) {
		file+= '<scene name="'+animation[1][i][0]+'">';
		for (j=0;j<animation[1][i][1].length;j++) {
			file+="<cuadro>";
				for (k=0;k<4;k++) {
					file+='<cube number="'+k+'">';
						for (l=0;l<64;l++) {
							file+= animation[1][i][1][j][k][l];
						}
					file+='</cube>';
				}
			file+="</cuadro>";	
		}
		file+= '</scene>';
	}
	file+= '</animation>';
	
	$.ajax({
        url:        "save.php",
        type:    	"POST",
        data:       "xml=" + file,
        complete:   function(e) { 
        	$("#save").fadeOut(300).fadeIn(10);
        }
    });
}
