 /**
 * Custom JS to realize infinite scroll on the Articles.
 */
(function(window, document, $, Drupal) {
  'usestrict';
  
  Drupal.behaviors.mfNodeCommonSponsored = {
    attach: function(context, settings) {
        // Load instagram script if undefined.
        if (typeof window.instgrm === 'undefined') {
            (function () {
                var s = document.createElement('script');
                s.async = true;
                s.src = '//platform.instagram.com/en_US/embeds.js';
                var x = document.getElementsByTagName('script')[0];
                x.parentNode.insertBefore(s, x);
            })();
        }
        // Load twitter script if undefined.
        if (typeof window.instgrm === 'undefined') {
            (function () {
                var s = document.createElement('script');
                s.async = true;
                s.src = '//platform.twitter.com/widgets.js';
                var x = document.getElementsByTagName('script')[0];
                x.parentNode.insertBefore(s, x);
            })();
        }
    }
  };

})(window, document, jQuery, Drupal);