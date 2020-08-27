<?php

/**
 * @file
 * MF Homepage Lear it, Earn it, Bearn it block template.
 */
?>
<div class="page-wrapper">
  <div class="block-mf-homepage-mf-homepage-promo-banners">
    <?php if (isset($desktop) && !empty($desktop)) : ?>
        <div class="desktop-promo-banner-block hide-small hide-small">
          <?php print $desktop; ?>
        </div>
      <?php endif; ?>
      <?php if (isset($mobile) && !empty($mobile)) : ?>
        <div class="mobile-promo-banner-block hide-desk hide-med">
          <?php print $mobile; ?>
        </div>
    <?php endif; ?>
  </div>
</div>
