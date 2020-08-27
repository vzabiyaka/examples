/**
 * Custom script to request and past items in not top news slider
 */

(function($, Drupal){
  /**
   * Helper fucntion to fetch the data
   */
  function fetchItems(url, container) {
    $.ajax({
      method: 'POST',
      url: url
    })
    .done(function(data) {
      container.append(data);
    });
  }
  /**
   * Behavior to fetch and attach data to the node top news block.
   */
  Drupal.behaviors.mfNodeTopSlider = {
    attach: function(context, settings) {
      var container = $('.node-top-slider-holder'),
          timeoutID,
          isSponsored = utag_data.sponsored;
      // Return if no parent container or it's empty or the page is sponsored
      if(!container.length || isSponsored == 'yes' || typeof settings.mfNode === 'undefined') return;
      // Do it only once
      container.once('node-top-slider', function() {
        var nid = settings.mfNode.nid,
            url = settings.basePath + 'load/node-top-news/' + nid;
        // Remove static slider
        $(this).prev('.items-slider').remove();
        // Activete async slider
        fetchItems(url, $(this));
        // Using timeout to wait fot an object
        clearTimeout(timeoutID);
        timeoutID = setTimeout(function(){
          if(container.html()) {
            Drupal.behaviors.mfNodeTopSlider.makeSlider()
          }
        }, 1500);
      });
    },
    makeSlider: function() {
      var sliderCont = $('.items-slider');
      sliderCont.removeClass('is-hidden');
      // Define controls for each slider instance.
      var next = sliderCont.find('.items-slider__btn_next'),
          prev = sliderCont.find('.items-slider__btn_prev');
      // Do slider stuffs.
      var slider = sliderCont.find('.items-slider__items-list').lightSlider({
        slideMargin: 0,
        controls: false,
        pager: false,
        enableDrag: false,
        autoWidth: true,
        onAfterSlide: function (el) {
          if (el.getCurrentSlideCount() !== 1) {
            prev.prop('disabled', false);
          }
          if (slider.getCurrentSlideCount() === 1) {
            prev.prop('disabled', true);
          }
        }
      });
      // Make prev button inactive on load.
      if (slider.getCurrentSlideCount() === 1) {
        prev.prop('disabled', true);
      }
      // attach go to next slide action
      next.on('click', function() {
        slider.goToNextSlide();
      });
      // attach go to prev slide action
      prev.on('click', function() {
        slider.goToPrevSlide();
      });
    }
  }
})(jQuery, Drupal);