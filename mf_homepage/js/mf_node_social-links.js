(function($) {
  // Open popup windows in the center of screen.
  window.popupCenter = function(url, title, w, h) {
    // Fixes dual-screen position                         Most browsers      Firefox
    var dualScreenLeft = window.screenLeft !== undefined ? window.screenLeft : screen.left;
    var dualScreenTop = window.screenTop !== undefined ? window.screenTop : screen.top;

    width = window.innerWidth ? window.innerWidth : document.documentElement.clientWidth ? document.documentElement.clientWidth : screen.width;
    height = window.innerHeight ? window.innerHeight : document.documentElement.clientHeight ? document.documentElement.clientHeight : screen.height;

    var left = ((width / 2) - (w / 2)) + dualScreenLeft;
    var top = ((height / 2) - (h / 2)) + dualScreenTop;
    var newWindow = window.open(url, title, 'scrollbars=yes, width=' + w + ', height=' + h + ', top=' + top + ', left=' + left);

    // Puts focus on the new Window
    if (window.focus) {
      newWindow.focus();
    }
  };


  Drupal.behaviors.mfNodeSocialLinksBar = {
    attach: function(context) {
      $('.article-full__social-links-block', context).once(function() {
        var btn = $(this).find('.social-links-block__btn');
        btn.bind('click', function(event) {
          var url = $(this).data('url');
          // Show popup window on social links click.
          window.popupCenter(url, '', 520, 550);
        });
      });
    }
  };

  Drupal.behaviors.mfNodeSocialTealiumLinks = {
    attach: function(context) {
      $('.social-links-block__btn').click(function() {
        var label = this.title;
        utag.link(
            { event_category: "social", event_action: "share", event_label: label }
        );
      });

    }
  };

})(jQuery);