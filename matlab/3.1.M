n = 10
A = zeros(n,n);
 for i=1:n
    for j=i:n
      if j == i
          A(i,j) = 0;
      else
          A(i,j) = randi(2)-1;
          A(j,i) = A(i,j);
      end
    end
end
A
e = ones(n, 1)

c3 = (A.*(A*A)) * e ./ 2