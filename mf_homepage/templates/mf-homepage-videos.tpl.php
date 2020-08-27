<?php

/**
 * @file
 * MF Homepage editors picks block template.
 */
?>

<?php if (!empty($big_video) || !empty($small_videos)): ?>
  <?php if (!empty($big_video)): ?>
    <div class="videos-hero-block__item videos-hero-block__item_main">
      <?php print render($big_video); ?>
    </div>
  <?php endif; ?>
  <?php if (!empty($small_videos) && is_array($small_videos)): ?>
    <div class="block-scrollable videos-hero-block__item videos-hero-block__item_secondary">
      <?php foreach ($small_videos as $item): ?>
          <?php print render($item); ?>
      <?php endforeach; ?>
    </div>
  <?php endif; ?>
<?php endif; ?>
