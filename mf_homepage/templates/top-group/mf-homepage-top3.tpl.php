<?php

/**
 * @file
 * MF Homepage module top3 template.
 */
?>

<div class="main-hero-block <?php print $class; ?>">
  <?php if (!empty($top)): ?>
    <div class="main-hero-block__item">
      <?php print render($top); ?>
    </div>
  <?php endif; ?>
  <?php if (!empty($bottom) && is_array($bottom)): ?>
    <?php foreach ($bottom as $key => $item): ?>
      <?php if ($key == $sharethrough_en) : ?>
        <div class="main-hero-block__item hide-desk hide-med">
          <?php print render($item); ?>
        </div>
      <?php else : ?>
        <div class="main-hero-block__item<?php if ($key == 'distroscale') : print ' distroscale-item'; endif;?>">
          <?php print render($item); ?>
        </div>
      <?php endif; ?>
      <?php endforeach; ?>
  <?php endif; ?>
</div>
