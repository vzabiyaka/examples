<?php

/**
 * @file
 * MF homepage Top Group block template.
 */
?>

<?php if (!empty($content)): ?>
  <div class="page__blocks-set page-wrapper cf">
    <div class="l-main l-main_three-cols cf">
      <?php if (!empty($content['left'])): ?>
        <div class="l-main__sidebar-first">
          <?php foreach ($content['left'] as $left_block): ?>
            <?php print $left_block; ?>
          <?php endforeach; ?>
        </div>
      <?php endif; ?>
      <?php if (!empty($content['center'])): ?>
        <div class="l-main__main-col">
          <?php foreach ($content['center'] as $center_block): ?>
            <?php print $center_block; ?>
          <?php endforeach; ?>
        </div>
      <?php endif; ?>
      <?php if (!empty($content['right'])): ?>
        <div class="l-main__sidebar-second">
          <?php foreach ($content['right'] as $right_block): ?>
            <?php print $right_block; ?>
          <?php endforeach; ?>
        </div>
      <?php endif; ?>
    </div>
  </div>
<?php endif; ?>
