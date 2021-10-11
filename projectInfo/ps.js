function loadf(){
	startTab=2;
	a=["file","class","function"]

	//~ for(i=0;i<3;i++){
		//~ console.log(Object.keys(ev(i)[0]))
	//~ }
	
	s=s1='';
	for(i=0;i<a.length;i++){
		s+='<button class="tablinks" onclick="tab(event,'+i+')" id="b'+i+'">'+a[i]+' information</button>'
		s1+='<div id="d'+i+'" class="tabcontent"><table id="table'+i+'" class="c"></table></div>'
	}
	el('d').innerHTML=s;
	el('s').innerHTML=s1;
	
	a=[
		['N','name','size','lines','last modification time'],
		['class / struct / union / enum class','functions','file line','children directly','children all'],
		['function signature','file line']
	];
	
	totalFunctions=0;
	ev(1).forEach((e) => {
		totalFunctions+=e.c;
	});	

	sz=0
	ln=0
	ev(0).forEach((e) => {
		sz+=e.size;
		ln+=e.lines;
	});
	
	prepare(ev(0))
	//console.log(Object.keys(ev(0)[0]))
	
	a.forEach((b,n) => {
		o=ev(n)
		s='';
		b.forEach((e,i) => {
			if(n==0 && i>=1 && i<=3 || n==1 && i<2 ){
				if(n==0){
					v=i==1?o.length:(i==2?sz:ln)
				}
				else{
					v=i==0?o.length:totalFunctions
				}
				e+=' : '+formatString(v)
			}
			if(n==0 && i==0){
				s+='<th>'+e
			}
			else{
				s+=	sortColumn(e+'&nbsp',sortf,n,i)
			}
		});
		insertRowsCells(table(n),s,o.length,b.length);
		fillf(n)
	});

	//default tab
	el('b'+startTab).click()
	
}

function prepare(a){
	a.forEach((e) => {
		//set e.times for sort
		e.times=new Date(e.time);
		//change e.time for output
		e.time=formatDate(e.times)
		//set e.names for sort
		e.names=e.name.substr(e.name.lastIndexOf("/")+1)
	});
}
 
function load(){
	gca=[]
	for(k in gd[0]){
		if(k!='a'){
			gca.push(k)
		}
	}
	
	gc=['N']
	for(k in gd[0].a[0]){
		gc.push(k)
	}
	
	a=[gd.length,0,0,0];
	gd.forEach((e) => {
		a[1]+=e.files;
		a[2]+=e.size;
		a[3]+=e.lines;
		prepare(e.a)		
	});	
	
	s=''
	gca.forEach((e,i) => {
		//first paremeter is ignored
		s+=	sortColumn(gca[i],sorta,0,i)
	});
	t=el('main')
	insertRowsCells(t,s,gd.length,gca.length+2);
	
	addResume(t)		

	i=a.length
	for(n=0;n<gd.length;n++){
		r=t.rows[n+1]
		r.cells[i].innerHTML="<button onclick='toggleDiv("+n+")'><img id='img"+n+"' src='plus.png'></button>"
		r.cells[i+1].innerHTML="<table id='table"+n+"' class='hide'></table>"		
	}

	filla();
		
	//~ s=''
	//~ el('p').innerHTML="["+s+"]";
}

function addResume(t){
	var s=''
	gca.forEach((e,i) => {
		s+='<th>';
		if(i==0){
			s+='total';
		}
		else{
			s+= gca[i];
		}
	});
	t.insertRow(-1).innerHTML=s;
		
	var r=t.insertRow(-1)
	a.forEach((e) => {
		r.insertCell(-1).innerHTML=formatString(e)
	});
}
	
function insertRowsCells(t,s,rows,cells){
	var i,j,r;
	t.insertRow(-1).innerHTML=s;
	for(i=0;i<rows;i++){
		r=t.insertRow(-1)
		for(j=0;j<cells;j++){
			r.insertCell(-1);
		}
	}
}

//from jm.js many modifications
function sortColumn(name,sortf,i,j){
	var s="<th><table align='center' class='nb sort'>";
	var f=(i,j,k,fn)=>"<td><img src='"+(k==0?'up':'down')+"8.png' onclick='"+fn+"("+i+","+j+","+k+")' >"
	for(var k=0;k<2;k++){
		s+="<tr>";
		if(k==0){
			s+="<th rowspan=2>"+name;
		}
		s+=f(i,j,k,sortf.name)
	}
	return s+'</table>';
}

function filla(){
	t=el('main');
	
	for(n=0;n<gd.length;n++){
		//need to create tables of every project after sorting of projects, 
		//because number of rows depends on the project
		createTable(n)
		
		gca.forEach((e,i) => {
			s=gd[n][e];
			if(typeof s=='number'){
				s=formatString(s)
			}
			t.rows[n+1].cells[i].innerHTML=s;
		});
		
		fill(n)
	}
}

function createTable(n){
	var s=''		
	gc.forEach((e,i) => {
		if(i==0){
			s+='<th>'+e;
		}
		else{
			s+=	sortColumn(e=='time'?'modification time':e,sort,n,i)
		}
	});		
	insertRowsCells(table(n),s,gd[n].a.length,gc.length);
}

function fill(n){
	var r,s;
	var t=table(n);
	gd[n].a.forEach((b,i) => {
		r=t.rows[i+1];
		gc.forEach((e,j) => {
			if(j==0){
				s=i+1;
			}
			else{
				s=b[e];				
				if(typeof s=='number'){
					s=formatString(s)
				}
			}
			r.cells[j].innerHTML=s;
		});
	});
}
	
function fillf(n){
	var i,j,r,t,q
	t=table(n);
	ev(n).forEach((b,i) => {
		r=t.rows[i+1];
		if(n==0){
			r.cells[0].innerHTML=i+1;
		}
		//use just first r.cells.length other fields is used for sorting functions and should not be shown
		for(j=(n==0?1:0);j<r.cells.length;j++){
			q=b[Object.keys(b)[n==0?j-1:j]]
			r.cells[j].innerHTML=typeof q=='number'?formatString(q):q;
		}
	});
}
	
//from jm.js 
/*
n - table number
c - column number
o - order type
*/
function sort(n,c,o){
	key=gc[c];
	if(key=='time'){
		key='times';
	}
	t=typeof gd[n].a[0][key]=='string'
	gd[n].a.sort((a, b)=>{
		v=t ? a[key].localeCompare(b[key]) : a[key] - b[key];
		if(o){
			v=-v;
		}
		return v;
	});
	fill(n);	
}

/*
n - ignored
c - column number
o - order type
*/
function sorta(n,c,o){
	key=gca[c];
	t=typeof gd[0][key]=='string'
	gd.sort((a, b)=>{
		v=t ? a[key].localeCompare(b[key]) : a[key] - b[key];
		if(o){
			v=-v;
		}
		return v;
	});
	
	//clear all projects tables
	for(n=0;n<gd.length;n++){
		if(getButton(n)[1]){//close all opened tables
			toggleDiv(n)
		}
		table(n).innerHTML = ""
	}
	
	filla();	
}

function sortf(n,c,o){
	a=ev(n)
	b=Object.keys(a[0]);
	
	if(n==0){
		//for first table additional column 'N'
		c--;
	}
	key=b[c];
	
//	console.log(key)
	
	//if has special field for sorting info -> infos
	if(b.includes(key+'s')){
		key+='s';
	}
	
//	console.log(n,c,key)
	
	t=typeof a[0][key]=='string'
	a.sort((a, b)=>{
		v=t ? a[key].localeCompare(b[key]) : a[key] - b[key];
		if(o){
			v=-v;
		}
		return v;
	});
	fillf(n);	
}

function toggleDiv(i){
	j=getButton(i)
	open=j[1]
	j[0].src = open?'plus.png':'minus.png'
	table(i).style.display = open ? 'none':'table-cell'
}

function getButton(i){
	img=el('img'+i);
	s=img.src
	p='plus.png'
	open = s.substring(s.length - p.length)!=p
	return [img,open]
}

function el(i){
	return document.getElementById(i);
}

//0 -> g0, 1->g1, index to variable
function ev(i){
	return eval('g'+i);
}

function table(i){
	return el('table'+i);
}

function formatDate(t){
	//var m=["JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"]
	var m=["jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"]
	var f=(i) =>String(i).padStart(2, '0')
	return f(t.getDate()) + ' ' + m[t.getMonth()] + ' ' +t.getFullYear()+" "+f(t.getHours())+":"+f(t.getMinutes())+":"+f(t.getSeconds());
}

//from jm.js 1234567890.323 -> 1 234 567 890.323
function formatString(n,separator){
	var s=n.toString();
	return s.replace(s.indexOf('.')==-1 ? /\B(?=(\d{3})+$)/g : /\B(?=(\d{3})+\.)/g, typeof separator=='undefined' ? " ":separator);
}

/*https://www.w3schools.com/howto/howto_js_tabs.asp*/
function tab(evt, n) {
  // Declare all variables
  var i, tabcontent, tablinks;

  // Get all elements with class="tabcontent" and hide them
  tabcontent = document.getElementsByClassName("tabcontent");
  for (i = 0; i < tabcontent.length; i++) {
    tabcontent[i].style.display = "none";
  }

  // Get all elements with class="tablinks" and remove the class "active"
  tablinks = document.getElementsByClassName("tablinks");
  for (i = 0; i < tablinks.length; i++) {
    tablinks[i].className = tablinks[i].className.replace(" active", "");
  }

  // Show the current tab, and add an "active" class to the button that opened the tab
  el('d'+n).style.display = "block";
  evt.currentTarget.className += " active";
}
