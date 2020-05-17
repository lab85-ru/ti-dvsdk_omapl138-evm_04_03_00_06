function toggle(e) {
  if( e.className == 'open' )
    e.className = 'closed';
  else if (e.className == 'closed')
    e.className = 'open';
}

function cancel(e) {
  if( window.event )
    window.event.cancelBubble = true;  // ie
  else if (e.stopPropagation) 
    e.stopPropagation();  // firefox
}
