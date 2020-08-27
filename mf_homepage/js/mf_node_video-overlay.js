/**
* Custom JS to realize video overlays on the page.
*/
(function(window, document, $, Drupal) {
  'usestrict';
  // Define need static values.
  var mobileBreakPoint = 768;
  
  /**
   * [getViewPortSize helps get current window width]
   * @return {number} a value of the current viewport width
   */
  function getViewPortSize() {
    return window.document.documentElement.clientWidth;
  }
  
  /**
   * [isTablet chack if we are in tablet viewport]
   * @return {Boolean} true if we are in tablet viewport
   */
  function isTablet() {
    var size = getViewPortSize();
    return  mobileBreakPoint <= size && size < 1024;
  }
  
  /**
   * generateS takes a string and returns a part of string by `/` split (node url).
   * @param  {string} str incomming string (node url).
   * @param  {number} index a number of the index array to take
   * @return {string}  nes s value
   */
  function generateS(str, index) {
    var tempArray = str.split('/');
    var newStr = tempArray[index];
    return newStr;
  }
  
  /**
   * generateAdUnit generates adUnit string
   * @param  {string} str incomming string (node url).
   * @return {string}  new adUnit value
   */
  function generateAdUnit(str) {
    var tempArray = str.split('/');
    var newStr = '/' + tempArray[1] + '/' + tempArray[2];
    return newStr;
  }
  
  Drupal.behaviors.mfNodeVideoOverlay = {
    attach: function(settings, context) {
      var $workoutNode = $('.workout-routine');
      
      // Add need static settings.
      var curArt = $workoutNode[0];
      // Take current node path string to retrieve need data.
      var path = curArt.getAttribute('about');
      // Take a node type data.
      var nType = curArt.getAttribute('data-node-type');
      // Define new node id.
      var newNid = curArt.getAttribute('id').replace('-', '_');
      
      var adUnit = generateAdUnit(path);
      
      // Store a need for ads data in settings.
      Drupal.settings.mfNodeVideoOverlay = {
        'targeting': {
          's1': generateS(path, 1),
          's2': generateS(path, 2),
          'pid': newNid,
          'ctype': nType
        },
        'adUnit': adUnit
      };
      if(window.innerWidth < 768) {
        $workoutNode.once('mobile-video', function() {
          var videoLink = $(this).find('.video-mobile-link');
          // Attach event for each link.
          videoLink.each(function() {
            var url = $(this);
            Drupal.behaviors.mfNodeVideoOverlay.getDataMobile(url, $workoutNode);
          });
        })
      } else {
        $workoutNode.once('video-overlay', function() {
          var videoLink = $(this).find('.video-overlay-link');
          // Attach event for each link.
          videoLink.each(function() {
            $(this).on('click', function(event){
              event.preventDefault();
              var url = $(this).attr('href');
              $('#video-overlay-container').addClass('is-loading');
              Drupal.behaviors.mfNodeVideoOverlay.getData(url, $workoutNode);
            });
          });
        });
      }
    },
    getData: function(url, node) {
      var $overlayContainer = $('#video-overlay-container'),
          path = url,
          art = node;
      $.get(path, function(data) {
        var $data = data;
        $('body').addClass('video-overlay-opened');
        $overlayContainer.append($data);
        $overlayContainer.removeClass('is-loading');
        $overlayContainer.addClass('is-loaded');
        Drupal.behaviors.mfNodeVideoOverlay.attachAds(art);
        Drupal.behaviors.mfNodeVideoOverlay.closeOverlay();
      });
    },
    getDataMobile: function(url, node) {
      var path = url.attr('href'),
          videoContainer = url.parent();
      $.get(path, function(data) {
        var $data = data;
        $(videoContainer).prepend($data).addClass('video-proceeded');
      });
    },
    closeOverlay: function() {
      var $closeBtn = $('.video-overlay__close-btn');
      $closeBtn.on('click', function() {
        var parent = $(this).parent();
        parent.remove();
        $('body').removeClass('video-overlay-opened');
        $('#video-overlay-container').removeClass('is-loaded');
      });
    },
    /**
     * attachAds the main realization idea taken from 
     * https://support.google.com/dfp_premium/answer/4578089?hl=en
     * `Implementing tags on pages with infinite contents`
     * @param  {object} el  an article node element.
     * @returns {void}
     */
    attachAds: function (el) {
        googletag.cmd.push(function () {
            //  Redefine publisher data. {static}
            var pubId = '4216/mensfitness';
            // Slot id increment. already dynamic.
            var nextSlotId = getRandomArbitrary(1, 20).toString();

            // Bottom container width.
            var bottmContWidth = el.find('.video-overlay__footer').width();

            // Bottom container height.
            var topContHeight = el.find('.video-overlay__footer').height();

            // Define slot sizes.
            var topSlotSizes = function () {
                if (bottmContWidth < 970) {
                    return [[728, 90]];
                }
                else {
                    return [[728, 90], [970, 66], [970, 90]];
                }
            };

            var rightSlotSizes = function () {
                if (topContHeight < 600) {
                    return [[300, 250], [300, 251]];
                }
                else {
                    return [[300, 250], [300, 251], [300, 600]];
                }
            };

            /**
             * [getRandomArbitrary return a random string between min and max]
             * @param  {number} min [range start point]
             * @param  {number} max [range end point]
             * @return {number}
             */
            function getRandomArbitrary(min, max) {
                return Math.round(Math.random() * (max - min) + min);
            }

            /**
             * generateNextSlotName helper function to generate new unique slot name
             * @param  {string} slot slot name prefix, defined from a given slot
             * @param  {nember} ofs given offset number from incoming parrams
             * @return {string} new string for a slot id
             */
            function generateNextSlotName(slot, ofs) {
                var id = ofs;
                return slot + '_' + id;
            }

            // Create slots
            var slot1 = generateNextSlotName('dfp-ad-right1_300x250', nextSlotId);
            var slot2 = generateNextSlotName('dfp-ad-top_728x90', nextSlotId);
            // Prepare places for a slots.
            el.find('.video-overlay__right-ad').append('<div class="block-ami-dfp-blocks"><div class="content"><div id="' + slot1 + '"></div></div></div>');
            el.find('.video-overlay__bottom-ad').append('<div class="block-ami-dfp-blocks"><div class="content"><div id="' + slot2 + '"></div></div></div>');
            // Define new ad unit
            var adUnit = Drupal.settings.mfNodeVideoOverlay.adUnit;
            // Set targeting.
            Object.keys(Drupal.settings.mfNodeVideoOverlay.targeting).map(function (objectKey, index) {
                googletag.pubads().setTargeting(objectKey, Drupal.settings.mfNodeVideoOverlay.targeting[objectKey]);
            });
            window.videoOverlaySlotsIds = [];
            if (isTablet()) {
                var slot_top = googletag.defineSlot(pubId + adUnit, topSlotSizes(), slot2).setTargeting("pos", "top").addService(googletag.pubads());
                window.videoOverlaySlotsIds.push(slot2);
            }
            else {
                var slot_top = googletag.defineSlot(pubId + adUnit, topSlotSizes(), slot2).setTargeting("pos", "top").addService(googletag.pubads());
                window.videoOverlaySlotsIds.push(slot2);

                var slot_right = googletag.defineSlot(pubId + adUnit, rightSlotSizes(), slot1).setTargeting("pos", "right1").addService(googletag.pubads());
                window.videoOverlaySlotsIds.push(slot1);
            }
            googletag.enableServices();
        });
        pbjs.que.push(function () {
            pbjs.refreshAds(window.videoOverlaySlotsIds);
        });
    }
  };
})(window, document, jQuery, Drupal);