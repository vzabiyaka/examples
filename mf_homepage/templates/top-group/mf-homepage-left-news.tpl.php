<?php

/**
 * @file
 * MF Homepage left news block template.
 */
?>

<?php if (!empty($items)): ?>
  <ul class="items-list <?php print $class; ?>">
    <?php if (!empty($top_node)): ?>
      <li class="items-list__item">
        <?php print render($top_node); ?>
      </li>
    <?php endif; ?>
    <?php foreach ($items as $item): ?>
      <li class="items-list__item">
        <a href="<?php print $item['path']; ?>">
          <div class="media-block__media">
            <?php if (!empty($item['image'])): ?>
              <?php print $item['image']; ?>
            <?php endif; ?>
          </div>
          <div class="media-block__body">
            <?php if (!empty($item['title'])): ?>
              <span class="items-list__title"><?php print $item['title']; ?></span>
            <?php endif; ?>
            <?php if (!empty($item['topics'])): ?>
              <span class="category-link <?php !empty($item['class']) ? print $item['class'] : ''; ?>">
                <?php print $item['topics']; ?>
              </span>
            <?php endif; ?>
          </div>
        </a>
      </li>
    <?php endforeach; ?>
  </ul>
<?php endif; ?>
