var number = 4;
var tabs = null;
var num = 4;
$(document).ready(function(){
	tabs=$('#tabs').scrollTabs({
		click_callback: function(e){
			$('div[id^="tab"]').addClass("hide").removeClass("active");
			$('#tabs-container').removeClass("hide");

			var id = $(this).children().attr('href');
			$(id + '.tab-content').removeClass("hide");
			$(id + '.tab-content').addClass("active");

		}
	});
});


function forwardFunction()
{
	if(number<16){
		++number; 
		tabs.addTab("<li id='li_"+number+"'><a id ='"+number+"' href='#tab"+number+"'>Zone "+number+"</li>");

		if(number>0){
			$("#button2").show()
		}
	}
	else{
		$("#button").hide();   
	}

}

function deleteTabs()
{ 

	
	if(number>1){
		tabs.removeTabs("#li_"+number);
		--number;

		if(number<17){
			$("#button").show(); 
		}
	}
	else{

		tabs.removeTabs("#li_"+number);
		--number;

		$('.scroll_tab_left_finisher').remove();
		$('.scroll_tab_right_finisher').remove();
		$("#button2").hide();
	}


}
function submit(){
	
	var myForm = $("#myForm").serializeJSON();
	var jsonString = JSON.stringify(myForm);
	//console.log(jsonString);

	$.ajax({
		url: "../savejson.php",
		cache: false,
		dataType: "text",
		type: 'POST',
		data: {jsonString: jsonString},
		success: function(data) {
			alert(data);
		}
		

	});

}


function submit1(){
	var myForm = $("div.active :input").not(value="").serializeJSON();
	var jsonString = JSON.stringify(myForm);
	//console.log(jsonString);

	$.ajax({
		url: "../savejson.php",
		cache: false,
		contentType: "application/json",
		type: 'POST',
		data: ({items: jsonString}),
		success: function(response) {
			alert(response);
		}

	});

}

function addEvent1(){
	var active = $('div').hasClass("tab-content card-content active");
	var openEvents1
	if(active){
	if(num<21){
		($('div.active').find('#row'+num+'')).removeClass('hide');
		++num;
	}else{
		$('#row'+num+'').removeClass('hide');
		$('#button4').addClass('hide');
	}
}
}

function switchCheck(){

	var checkbox = $('#mySwitch').prop('checked');
	var string = JSON.stringify(checkbox);
	console.log(string);	
	//alert('checkbox value: '+ checkbox);
	$.ajax({
		url: "../savejson.php",
		cache: false,
		dataType: "text",
		type: 'POST',
		data: {string: string},
		success: function(data) {
			alert(data);
		}
		

	});

}



function populate(frm, data) {
  
}




