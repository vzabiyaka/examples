<?php

/**
 * @file
 * MF Homepage module Today's Workout block template.
 */
?>

<div class="todays-workout-block">
  <strong class="todays-workout-block__title"><?php print t("Today's Workout");?></strong>

  <?php if (!empty($date)): ?>
  <span class="todays-workout-block__date"><?php print $date; ?></span>
  <?php endif; ?>

  <?php if (!empty($image) && !empty($node_path)): ?>
    <div class="todays-workout-block__media">
      <a href="/<?php print $node_path; ?>">
        <?php print $image; ?>
      </a>
    </div>
  <?php endif; ?>

  <?php if (!empty($title) && !empty($node_path)): ?>
      <a href="/<?php print $node_path; ?>">
        <strong class="todays-workout-block__headline">
          <?php print $title; ?>
        </strong>
      </a>
  <?php endif; ?>
  <a class="todays-workout-block__more" href="/training"><?php print t('See all'); ?></a>
</div>
