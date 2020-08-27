jQuery(function($){
  // Detect scroll direction 
  var lastScrollTop = 0,
      currentScroll;
  $(window).scroll(function(event){
     var st = $(this).scrollTop();
     if (st > lastScrollTop){
         scrollUp = false;
     } else {
        // upscroll code
        scrollUp = true;
     }
     lastScrollTop = st;
  });

  $(document).ready(function(){
    if(window.innerWidth > 767) {

      setTimeout(function(){
      $('.jwplayer').each(function(){
        var parent = $(this).closest('.article-item-wrapper'),
          videoHeight = $(this).height(),
          headerHeight = $('.site-header').height();

        $(this).hcSticky({
          top: 91,
          bottomEnd: 400,
          stickTo: parent,
          onStart: function(){
            if(scrollUp == false) {
              var st = $(window).scrollTop();

              $('html, body').animate({ scrollTop: st + videoHeight}, '100');
            }
          }
        });
      });
      }, 500);
    }
  });
});