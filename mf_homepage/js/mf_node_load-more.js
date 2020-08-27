 /**
 * Custom JS to realize infinite scroll on the Articles.
 */
(function(window, document, $, Drupal) {
  'usestrict';
  
  Drupal.behaviors.mfNodeInfinitScroll = {
    attach: function(context, settings) {
      $('.articles-wrapper').once('article-infs', function() {
        var viewport = window.innerWidth,
            startPoint = 1023,
            tid = settings.mfNode.tid,
            nid = settings.mfNode.nid,
            offset = 3, // Equal to the items Top 3 block starting from 0.
            quant = 1, // Quantity nodes to load.
            nodeTypes = 'article,recipe,video',
            target = $(this);
        var url = function(tid, nid, offset, quant, nodeTypes) {
          return settings.basePath + 'content/load?tids=' + tid + '&qt=' + quant + '&vm=full&offset=' + offset + '&not_in=' + nid + '&types=' + nodeTypes + '&no_wrap=TRUE&not_sponsored=TRUE';
        };
        // Adding offset to tealium utag_data for first node.
        utag_data.offset = offset;
        // First adding the initial load more link.
        target.after('<a class="infinite-more-link" href="' + url(tid, nid, offset, quant, nodeTypes) + '"><span>L</span><span>o</span><span>a</span><span>d</span><span>i</span><span>n</span><span>g</span></a>');
        // Mark first node ad 'Down proceed', because we don't need any extra actions with it.
        target.find('.article-item-wrapper').addClass('down-proceed');
        // We need it only on desktops.
        // Add Instagremm script, because we need it.
        Drupal.behaviors.mfNodeInfinitScroll.addInstagram();
        // Add Instagremm twitter, because we need it.
        Drupal.behaviors.mfNodeInfinitScroll.addTwitter();
        if (viewport > startPoint) {
          // Then init the infinite pages loader.


          $('body').on('stickyTimerEvent', function(){
            Drupal.behaviors.mfNodeInfinitScroll.updateHcSticky();  
          });


          var infinite = new Waypoint.Infinite({
          element: target[0],
          items: '.article-item-wrapper',
          onBeforePageLoad: function() {
            // Increase offset value
            offset ++;
            // And update more link href.
            target.next('.infinite-more-link').attr('href', url(tid, nid, offset, quant, nodeTypes));
          },
          onAfterPageLoad: function() {
            $('.article-item-wrapper').each(function(element, index) {
              // Init waypoint.
              $(this).waypoint({
                handler: function(direction) {
                  var el = $(this.element);
                  // Down direction triggers proceed.
                  if (direction === 'down') {
                    // Updated history state on each down move.
                    Drupal.behaviors.mfNodeInfinitScroll.updateHistory(el);
                    // Do other stuffs only once.
                    if(!el.hasClass('down-proceed')) {
                      // Update first letter.
                      Drupal.behaviors.mfNodeInfinitScroll.firstLetter(el.find('.article-full__article-body'));
                      // Fix broken JW video, loaded with infinite scroll
                      Drupal.behaviors.mfNodeInfinitScroll.videoUpdate(el.find('.page-wrapper'));
                      // Update Sticky video
                      Drupal.behaviors.mfNodeInfinitScroll.stickyVideo(el.find('.jwplayer_wrapper'));
                      // Init instagram embeds.
                      if (typeof window.instgrm !== 'undefined') {
                        window.instgrm.Embeds.process();
                      }
                      // Init facebook embeds.
                      if (typeof FB !== 'undefined' && FB.XFBML && FB.XFBML.parse()) {
                        FB.XFBML.parse();
                      }
                      // Init twitter embeds.
                      if (typeof twttr === 'object') {
                        twttr.widgets.load(el.find('.article-full')[0]);
                      }
                      // Activate social bar action in the loaded node.
                      Drupal.behaviors.mfNodeSocialLinksBar.attach();
                      // Attaching new ads set for a node.
                      Drupal.behaviors.mfNodeInfinitScroll.attachAds(el, offset);
                      // Update sticky points for a new containers.
                      Drupal.behaviors.mfNodeInfinitScroll.updateStickyPoints(el); 
                      // Update tealium utag_data object.
                      Drupal.behaviors.mfNodeInfinitScroll.tealiumUpdate();
                      // Update sticky leaderboard for a new containers.
                      Drupal.behaviors.mfNodeInfinitScroll.updateStickyLeaderboard(el);
                      // Mark that all stufs are done.
                      el.addClass('down-proceed generated');
                    }
                  }
                  if (direction === 'up') {
                    // Update sticky points for a new containers.
                      Drupal.behaviors.mfNodeInfinitScroll.updateStickyPoints(el); 
                    if (this.previous()) {
                      // Push history state.
                      Drupal.behaviors.mfNodeInfinitScroll.updateHistory($(this.previous().element));
                    }
                  }
                },
                offset: '100%'
              });

              $(this).once('comments', function() {
                  var that = $(this);

				  Drupal.behaviors.mfNodeInfinitScroll.scrollToComments(that);
			  });
            });
          },
          offset: function() {
            return (this.context.innerHeight()*2) - this.adapter.outerHeight();
          }
        });
        }
      });
    },
    /**
     * updateHistory helper function to updated browser history state.
     * @param  {object} el The element to work with.
     * @return {void}
     */
    updateHistory: function(el) {
      var elem = el,
          article = elem.find('.article-full')[0];
      var newTitle = elem.find('.article-header__title').text(),
          newUrl = article.getAttribute('about'),
          newNid = article.id.replace('node-', '');
      var newState = {
        nid: newNid,
        title: newTitle,
        url: newUrl
      };
      // Push title for Firefox.
      document.title = newTitle;
      // Update history state.
      window.history.pushState(newState, newTitle, newUrl);
    },
    firstLetter: function(article) {
      var container = article;
      if (container.length) {
        var insBlock = container.find('.article-full__first-letter');
        //charCodeAt() gets the char code in a string
        //Upper and lower bounds for upper case characters
        var upperBoundUpper = "A".charCodeAt(0);
        var lowerBoundUpper = "Z".charCodeAt(0);
        //Upper and lower bounds for lower case characters
        var upperBoundLower = "a".charCodeAt(0);
        var lowerBoundLower = "z".charCodeAt(0);
        if (!insBlock.length) {
          var firstParagraphText = container.find('.field-name-body p:first-of-type')[0].innerText,
              firstParagraph = container.find('.field-name-body p:first-of-type')[0];
          if (firstParagraph.innerHTML.length > 10) {
            for (var i = 0; i < firstParagraphText.length; i++) {
              var char = firstParagraphText.charCodeAt(i);
              if (char >= upperBoundUpper && char <= lowerBoundUpper || char >= upperBoundLower && char <= lowerBoundLower) {
                container.prepend('<div class="article-full__first-letter">'+ firstParagraphText[i] +'</div>');
                break;
              }
            }
            // Emphasize first 3 words of article body with different font MFXVI-575
            $(firstParagraph).html(function (i, html) {
              var words = html.split(/\s+/),
                  insertAfter = 3;

              for(i = 0; i<3; i++) {
                if(words[i].includes("<")) {
                  for(i = ++i; i<words.length; i++) {
                    
                    if(words[i].includes("</")) {
                      if(i < 3) {
                        insertAfter = 4;
                        break;
                      } else {
                        insertAfter = ++i;
                        break;
                      }
                    }
                  }
                }
              }

              return "<span class='emphasized-text'>" + words.slice(0, insertAfter).join(" ") + "</span> " + words.slice(insertAfter).join(" ");
            });
            //
          }
        }
      }
    },
    videoUpdate: function(container) {
      var videoWrapper = container.find('.jwplayer_wrapper');
          videoSrc = videoWrapper.attr('data-player-src'),
          scriptTag = document.createElement("script");

          scriptTag.type = "text/javascript";
          scriptTag.src = videoSrc;

      videoWrapper.append(scriptTag);
    },
    /**
    * Sticky video
    */
    stickyVideo: function(el) {
      el.each(function(){
        $('.jwplayer').each(function(){
          var that = this,
              parent = $(this).closest('.article-item-wrapper'),
              videoHeight = $(this).height(),
              headerHeight = $('.site-header').height();

          $(this).hcSticky({
            top: 91,
            bottomEnd: 400,
            stickTo: parent,
            onStart: function(){
              if(scrollUp == false) {
                var st = $(window).scrollTop();

                $('html, body').animate({ scrollTop: '+=240px'}, '100');
              }
            }
          });

          setInterval(function(){
            $(that).hcSticky('reinit')
          }, 1000);
        });
      });
    },
    /**
     * tealiumUpdate helper function for updating tealium vars object on infinite scroll.
     */
    tealiumUpdate: function() {
      var utag_data = $('.tealium-tags').last().data('json');
      if (typeof window.utag_data !== 'undefined') {
        utag_data.site_display_format = window.utag_data.site_display_format;
      }
      utag_data.event_name = 'infinityScrollPost';
      utag_data.referrer = window.location.pathname;
      utag_data.event_category = 'infinite_scroll';
      utag_data.event_label = 'infinityScrollPost';
      utag_data.event_action = 'next';
      utag_data.event_value = parseInt(utag_data.offset)-3+'';
      utag.view(utag_data);
        // Make an AJAX call to generate a response of 'pageview_candidate';
        $.get('/comscore/pageview-candidate', '', function (response) {
            // no output necessary
        });
    },
    addInstagram: function() {
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
    },
    addTwitter: function() {
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
    },
    /**
     * attachAds the main realization idea taken from 
     * https://support.google.com/dfp_premium/answer/4578089?hl=en
     * `Implementing tags on pages with infinite contents`
     * @param  {object} el  an article node element.
     * @param  {number} offset a number which will added to the generated Id
     * @returns {void}
     */
    attachAds: function(el, offset) {
        googletag.cmd.push(function() {
            //  Redefine publisher data. {static}
            var pubId = '4216/mensfitness';
            var curArt = el.find('.article-full')[0];
            // Take current node path string to retrieve need data.
            var path = curArt.getAttribute('about');
            // Take a node type data.
            var nType = curArt.getAttribute('data-node-type');
            // Define new node id.
            var newNid = curArt.getAttribute('id').replace('-', '_');
            // Slot id increment. already dynamic.
            var nextSlotId = offset;
            // Lazyload slots object.
            window.mfLoadMorelazyLoadSlots = window.mfLoadMorelazyLoadSlots || {count: 0};

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
             * generateNextSlotName helper function to generate new unique slot name
             * @param  {string} slot slot name prefix, defined from a given slot
             * @param  {nember} ofs given offset number from incoming parrams
             * @return {string} new string for a slot id
             */
            function generateNextSlotName(slot, ofs) {
                var id = ofs;
                return slot + '_' + id;
            }

            function generateWorksheetSlotName(slot, ofs) {
                var id = (ofs == 4) ? false : ofs - 1;
                return (id) ? slot + '_' + id : slot;
            }

            // Create slots
            var slot1 = generateNextSlotName('dfp-ad-right1_300x250', nextSlotId);
            var slot2 = generateNextSlotName('dfp-ad-right2_300x250', nextSlotId);
            var slot3 = generateNextSlotName('dfp-ad-top_728x90', nextSlotId);
            var worksheet_div_id_1 = 'dfp-ad-right1_300x250';
            var worksheet_div_id_2 = 'dfp-ad-right2_300x250';
            var worksheet_div_id_3 = 'dfp-ad-top_728x90';
            var slot4 = generateNextSlotName('dfp-ad-sharethrough', nextSlotId);
            // Prepare places for a slots.
            el.find('.rr-first-block-wrapper').append('<div class="block-ami-dfp-blocks"><div class="content"><div id="' + slot1 + '"></div></div></div>');
            // Temporary fix for oratv block.
            if ($('.rr-second-block-wrapper #custom_ora_block').length) {
                $('<div class="block-ami-dfp-blocks"><div class="content"><div id="' + slot2 + '"></div></div></div>').insertBefore(el.find('.rr-second-block-wrapper #custom_ora_block'));
            }
            else {
                el.find('.rr-second-block-wrapper').append('<div class="block-ami-dfp-blocks"><div class="content"><div id="' + slot2 + '"></div></div></div>');
            }
            el.find('.art-top-block-wrapper').append('<div class="block-ami-dfp-blocks"><div class="content"><div id="' + slot3 + '"></div></div></div>');

            // Define new ad unit
            var adUnit = generateAdUnit(path);
            // Now it's sample statick data, but will be changed dynamically for each node.
            var pageTargeting = {
                's1': generateS(path, 1),
                's2': generateS(path, 2),
                'pid': newNid,
                'ctype': nType,
                'strnativekey': 'rwj7RwEkJLy1HY5SmrbUXBPD'
            };
            // Set targeting.
            Object.keys(pageTargeting).map(function (objectKey, index) {
                googletag.pubads().setTargeting(objectKey, pageTargeting[objectKey]);
            });
            window.mfLoadMorelazyLoadSlots[slot1] = {slot: slot1, worksheet_div_id: worksheet_div_id_1, el: document.getElementById(slot1), adUnitPath: pubId + adUnit, pos: "right1", tablet_size: [[300, 250], [300, 251], [300, 600], [300, 1050]], desktop_size: [[300, 250], [300, 251], [300, 600], [300, 1050]]};
            window.mfLoadMorelazyLoadSlots[slot2] = {slot: slot2, worksheet_div_id: worksheet_div_id_2, el: document.getElementById(slot2), adUnitPath: pubId + adUnit, pos: "right2", tablet_size: [[300, 250], [300, 252]], desktop_size: [[300, 250], [300, 252]]};
            window.mfLoadMorelazyLoadSlots[slot3] = {slot: slot3, worksheet_div_id: worksheet_div_id_3, el: document.getElementById(slot3), adUnitPath: pubId + adUnit, pos: "top", tablet_size: [[728, 90], [970, 66], [970, 90]], desktop_size: [[728, 90], [970, 66], [970, 90]]};
            // window.mfLoadMorelazyLoadSlots[slot4] = {slot: slot4, el: document.getElementById(slot4), adUnitPath: pubId + adUnit, pos: "sharethrough", tablet_size: [2, 3], desktop_size: [2, 3]};

            window.mfLoadMorelazyLoadSlots.count = 3;

            window.addEventListener('scroll', Drupal.behaviors.mfNodeInfinitScroll.mfLoadMoreScrollListener, false);
        });
    },
      mfLoadMoreScrollListener: function() {
          if(typeof window.mfLoadMorelazyLoadSlots !== 'undefined') {
              for (var divId in mfLoadMorelazyLoadSlots) {
                  if (divId === 'count' || !elementInViewport(mfLoadMorelazyLoadSlots[divId].el)) {
                      continue;
                  }
                  window.mfLoadMoredivId = divId;
                  googletag.cmd.push(function () {
                      var mfLoadMorelazyloadSlotSize = mfLoadMorelazyLoadSlots[window.mfLoadMoredivId]['desktop_size'];

                      googletag.slots[window.mfLoadMoredivId] = googletag.defineSlot(mfLoadMorelazyLoadSlots[window.mfLoadMoredivId]['adUnitPath'], mfLoadMorelazyloadSlotSize, window.mfLoadMoredivId)
                          .setTargeting("pos", mfLoadMorelazyLoadSlots[window.mfLoadMoredivId]['pos'])
                          .addService(googletag.pubads());
                      googletag.display(window.mfLoadMoredivId);
                  });
                  if (typeof pbjs.infiniteScroll !== 'undefined') {

                      pbjs.infiniteScroll([
                          { targetAdUnitCode: mfLoadMorelazyLoadSlots[window.mfLoadMoredivId]['worksheet_div_id'], newAdUnitCode: window.mfLoadMoredivId }
                      ], {changeCorrelator: true});
                  }
                  else {
                      googletag.cmd.push(function () {
                          if (typeof pbjs.refreshAds !== 'undefined') {
                              pbjs.que.push(function () {
                                  pbjs.refreshAds([window.mfLoadMoredivId], {changeCorrelator: true});
                              });
                          }
                          else {
                              googletag.pubads().refresh([googletag.slots[window.mfLoadMoredivId]], {changeCorrelator: true});
                          }
                      });
                  }
                  delete window.mfLoadMorelazyLoadSlots[divId];
                  window.mfLoadMorelazyLoadSlots.count--;
                  if (window.mfLoadMorelazyLoadSlots.count === 0) {
                      window.removeEventListener('scroll', Drupal.behaviors.mfNodeInfinitScroll.mfLoadMoreScrollListener, false);
                  }
              }
          }
      },
    updateStickyPoints: function(el) {
      // For Article pages.
      var nodeBlocks = [
        '.rr-first-block-wrapper .block-ami-dfp-blocks',
        '.rr-second-block-wrapper'
      ];
      var sponsoredHeight = $('.sponsored-content__header-wrapper').length ? $('.sponsored-content__header-wrapper')[0].clientHeight : 0,
          topOf = sponsoredHeight + 60;

      nodeBlocks.forEach(function(element, index, array){
        var block = el.find(element),
            parent = el.find(element).parents('.article-item-wrapper'),
            leaderboardHeight = parent.find('.art-top-block-wrapper').hasClass('sticky') ? (parent.find('.art-top-block-wrapper .block-ami-dfp-blocks').height() + 100) : 0;

        if (element === '.rr-first-block-wrapper .block-ami-dfp-blocks') {
          parent = el.find(element).parent('.rr-first-block-wrapper');

          block.hcSticky({
            top: topOf + leaderboardHeight,
            bottomEnd: 20,
            stickTo: parent
          });

        }
        else {
          block.hcSticky({
            top: topOf + leaderboardHeight,
            bottomEnd: 1920, // This is summ of first ad keeper and zergnet block.
            stickTo: parent
          });
          
        }
      });
      // Update top 3 sticky
      Drupal.behaviors.mfxviStickyTop.stick(el.find('.article-full__top-articles'), el.find('.article-full__inner'));
    },

    // TODO: Needs to be improved, as it was done late-late night, right
    // before reease
    updateStickyLeaderboard: function(el) {
      // Stickyness for ads placed between nodes (MF-1638)
      $('.art-top-block-wrapper').each(function(){
        var sec = Drupal.settings.leaderboardStickySec,
            parent = $(this).parent('.article-item-wrapper'),
            that = this;

        // Disable stickyness after timeout

        // Stop leaderboard's stickyness
        $('.region-leaderboard').addClass('leaderboard_not-sticky');
        
        // Start stickiness for ads
        $(that).hcSticky({
          top: 60,
          bottomEnd: 20,
          stickTo: parent,
          onStart: function(){
            Drupal.behaviors.mfNodeInfinitScroll.updateStickyPoints(el); 
            stickyTimeout = setTimeout(function(sec) {
              $(that).hcSticky('off');
              $('body').trigger('stickyTimerEvent');
              $(that).parents().find('.wrapper-sticky>div').animate({top:60});
              
            }, sec * 1000);
          },
          onStop: function(){
            clearTimeout(stickyTimeout);
          }
        });

        // Reinit hcsticky to let it calculate ad's height
        setTimeout(function(){
          $(that).hcSticky('reinit')
        }, 500);  
      });
    },

    // Update HC Sticky position after leaderboard unstick
    updateHcSticky: function(el) {
      $('.article-item-wrapper').each(function(element, index) {
        var el = $(this);
        // For Article pages.
        var nodeBlocks = [
          '.rr-first-block-wrapper .block-ami-dfp-blocks',
          '.rr-second-block-wrapper'
        ];
        var sponsoredHeight = $('.sponsored-content__header-wrapper').length ? $('.sponsored-content__header-wrapper')[0].clientHeight : 0,
            topOf = sponsoredHeight + 60;
        if($(el).hasClass('down-proceed')) {
          nodeBlocks.forEach(function(element, index, array) {
              var block = el.find(element),
                  parent = el.find(element).parents('.article-item-wrapper'),
                  leaderboardHeight = parent.find('.art-top-block-wrapper').hasClass('sticky') ? (parent.find('.art-top-block-wrapper .block-ami-dfp-blocks').height() + 100) : 0;

              if (element === '.rr-first-block-wrapper .block-ami-dfp-blocks') {
                  parent = el.find(element).parent('.rr-first-block-wrapper');

                  block.hcSticky({
                      top: 60,
                      bottomEnd: 20,
                      stickTo: parent
                  });

              } else {
                  block.hcSticky({
                      top: 60,
                      bottomEnd: 1920, // This is summ of first ad keeper and zergnet block.
                      stickTo: parent
                  });

              }
          });
          // Update top 3 sticky
          Drupal.behaviors.mfxviStickyTop.stick(el.find('.article-full__top-articles'), el.find('.article-full__inner'));
        }
      });
    },

	scrollToComments: function(el) {
      var article = el;

      article.find('.social-links-block__comments').click(function() {
        var commentsBlock = article.find('.spot-im-conversation-module');

        $('html, body').animate({
          scrollTop: commentsBlock.offset().top - 150
        }, 2000);
      });
    }

  };

  

})(window, document, jQuery, Drupal);